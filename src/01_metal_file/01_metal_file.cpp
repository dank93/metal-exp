#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <chrono>
#include <iostream>

constexpr size_t ARR_LENGTH = 1024;
constexpr size_t BUFFER_SIZE = sizeof(float) * ARR_LENGTH;

const char* ADDER_SRC = R"(
	kernel void add_arrays(device const float* inA [[buffer(0)]],
	                       device const float* inB [[buffer(1)]],
	                       device float* result [[buffer(2)]],
	                       uint index [[thread_position_in_grid]])
	{
        for (int i = 0; i < 100000; i++)
        {
	        result[index] += inA[index] + inB[index];
        }
	}
)";

int main() {
  // Get a handle on the GPU
  MTL::Device* gpu = MTL::CreateSystemDefaultDevice();

  // Compile source into metal library, then grab function handle
  NS::Error* compile_error = nullptr;
  MTL::Library* adder_lib =
      gpu->newLibrary(NS::String::string(ADDER_SRC, NS::UTF8StringEncoding),
                      nullptr, &compile_error);
  if (adder_lib == nullptr) assert(false);
  MTL::Function* adder_func = adder_lib->newFunction(
      NS::String::string("add_arrays", NS::UTF8StringEncoding));

  // Initialize a pipeline. This defines a function that the GPU
  // can run in parallel, calls to which (with concrete args) are placed in
  // command buffers by command encoders. Command buffers are placed in a
  // command queue to be scheduled
  NS::Error* pipeline_error = nullptr;
  MTL::ComputePipelineState* pipeline =
      gpu->newComputePipelineState(adder_func, &pipeline_error);
  if (pipeline == nullptr) assert(false);
  NS::UInteger max_threads_per_group =
      pipeline->maxTotalThreadsPerThreadgroup();
  std::cout << "Max threads per threadgroup: " << max_threads_per_group
            << std::endl;

  // Get a new command queue for this device. Many cmd buffers / encoders
  // can be pushed to this queue
  MTL::CommandQueue* cmd_q = gpu->newCommandQueue();

  // Grab a buffer to push to the queue, and then create an encoder to to
  // define what function (i.e. pipeline) and corresponding arguments should
  // be bundled into this cmd buffer
  MTL::CommandBuffer* cmd_buffer = cmd_q->commandBuffer();
  MTL::ComputeCommandEncoder* enc = cmd_buffer->computeCommandEncoder();
  enc->setComputePipelineState(pipeline);

  // Set buffers to alert the GPU that they have been modified
  // (necessary for ResourceStorageModeManaged)
  MTL::Buffer* a_buffer =
      gpu->newBuffer(BUFFER_SIZE, MTL::ResourceStorageModeManaged);
  MTL::Buffer* b_buffer =
      gpu->newBuffer(BUFFER_SIZE, MTL::ResourceStorageModeManaged);
  MTL::Buffer* res_buffer =
      gpu->newBuffer(BUFFER_SIZE, MTL::ResourceStorageModeManaged);
  a_buffer->didModifyRange(NS::Range::Make(0, a_buffer->length()));
  b_buffer->didModifyRange(NS::Range::Make(0, b_buffer->length()));
  res_buffer->didModifyRange(NS::Range::Make(0, res_buffer->length()));

  // Point the encoder at these buffers, at indices that match those
  // expected by the function this encoder represents
  enc->setBuffer(a_buffer, 0, 0);
  enc->setBuffer(b_buffer, 0, 1);
  enc->setBuffer(res_buffer, 0, 2);

  // Use the widest grid necessary and the widest thread group
  // possible
  enc->dispatchThreads(MTL::Size(ARR_LENGTH, 1, 1),
                       MTL::Size(max_threads_per_group, 1, 1));

  // Finally end the encoding and commit the command buffer to the
  // command queue, thereby scheduling it for execution
  enc->endEncoding();
  auto gpu_start = std::chrono::steady_clock::now();
  cmd_buffer->commit();
  std::cout << "Dispatched GPU work, waiting..." << std::endl;
  cmd_buffer->waitUntilCompleted();
  auto gpu_end = std::chrono::steady_clock::now();

  std::cout << "Done with all GPU work, now trying on CPU" << std::endl;
  auto cpu_start = std::chrono::steady_clock::now();
  for (size_t i = 0; i < ARR_LENGTH; i++) {
    for (size_t j = 0; j < 100000; j++) {
      ((float*)(res_buffer->contents()))[i] +=
          ((float*)(a_buffer->contents()))[i] +
          ((float*)(b_buffer->contents()))[i];
    }
  }
  auto cpu_end = std::chrono::steady_clock::now();

  std::cout << "GPU work took: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(gpu_end -
                                                                     gpu_start)
                   .count()
            << "ms, CPU work took: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(cpu_end -
                                                                     cpu_start)
                   .count()
            << "ms" << std::endl;
}
