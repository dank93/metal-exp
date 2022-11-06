#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include <ApplicationServices/ApplicationServices.h>

#include <iostream>
#include <string>

void fancyPrintLocation(CGPoint location) {
  std::string clearLastLine = ("\33[2K\r(");
  std::string printOut = clearLastLine + std::to_string((int)(location.x)) +
                         std::string(", ") + std::to_string((int)(location.y)) +
                         std::string(")");
  std::cout << printOut << std::flush;
}

CGEventRef myCGEventCallback(CGEventTapProxy proxy, CGEventType type,
                             CGEventRef event, void *refcon) {
  if (type != kCGEventMouseMoved) {
    return event;
  }

  CGPoint location = CGEventGetLocation(event);
  fancyPrintLocation(location);
  return event;
}

int main() {
  CGRect screenBounds = CGDisplayBounds(CGMainDisplayID());
  printf("The main screen is %dx%d\n", (int)screenBounds.size.width,
         (int)screenBounds.size.height);

  CFMachPortRef eventTap = CGEventTapCreate(
      kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
      (1 << kCGEventMouseMoved), myCGEventCallback, NULL);

  if (!eventTap) {
    fprintf(stderr, "failed to create event tap :/\n");
    exit(1);
  }

  CFRunLoopSourceRef runLoopSource =
      CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                     kCFRunLoopCommonModes);

  CGEventTapEnable(eventTap, true);

  CFRunLoopRun();
}
