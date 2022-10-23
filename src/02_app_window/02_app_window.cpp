#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <iostream>

class AppDelegate : public NS::ApplicationDelegate
{
    public:
        ~AppDelegate()
        {
            window_->release();
            view_->release();
        }

        void applicationWillFinishLaunching(NS::Notification* notification) final;
        void applicationDidFinishLaunching(NS::Notification* notification) final;
        bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) final
        {
            return true;
        }

    private:
        NS::Window* window_;
        MTK::View* view_;
};

int main() {
    AppDelegate app_del;

    NS::Application* pSharedApplication = NS::Application::sharedApplication();
    pSharedApplication->setDelegate(&app_del);
    pSharedApplication->run();
}

void AppDelegate::applicationWillFinishLaunching(NS::Notification* notification)
{
    NS::Application* app = 
        reinterpret_cast<NS::Application*>(notification->object());
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void AppDelegate::applicationDidFinishLaunching(NS::Notification* notification)
{
    NS::Application* app = 
        reinterpret_cast<NS::Application*>(notification->object());

    const CGRect frame{{100.0, 100.0}, {512.0, 512.0}};

    view_ = MTK::View::alloc()->init(frame, MTL::CreateSystemDefaultDevice());
    view_->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    view_->setClearColor(MTL::ClearColor::Make(1.0, 0.0, 0.0, 1.0));

    window_ = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false);
    window_->setTitle(NS::String::string(
                "Cool App",
                NS::StringEncoding::UTF8StringEncoding));
    window_->makeKeyAndOrderFront(nullptr);
    window_->setContentView(view_);

    app->activateIgnoringOtherApps(true);
}
