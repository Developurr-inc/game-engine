//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#include <PAL/window.h>
#include <PAL/memory.h>

#ifdef N_PLATFORM_APPLE

# include <crt_externs.h>
# include <copyfile.h>
# include <errno.h>

# import <Cocoa/Cocoa.h>
# import <Foundation/Foundation.h>
# import <QuartzCore/CAMetalLayer.h>
# import <QuartzCore/QuartzCore.h>

static bool1 platform_window_pump_messages(Window *window);

@class ApplicationDelegate;

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {
}
@end

@implementation ApplicationDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    @autoreleasepool {
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [NSApp postEvent:event atStart:YES];
    }

    [NSApp stop:nil];
}
@end

typedef struct macos_handle_info {
    uint32 dummy;
} macos_handle_info;

typedef struct InternalState {
    ApplicationDelegate *app_delegate;
    macos_handle_info handle;
    bool1 quit_flagged;
    uint8 modifier_key_states;
} InternalState;

Window *platform_window_create(
    const char *application_name,
    const int32 x,
    const int32 y,
    const int32 width,
    const int32 height
) {
    Memory *memory = platform_memory_create();

    Window *window = memory->allocate(sizeof(Window), false);
    if (! window) {
        // N_FATAL("Failed to allocate memory for platform state");
        return nullptr;
    }

    InternalState *internal_state = memory->allocate(sizeof(InternalState), false);
    if (! internal_state) {
        memory->free(window, false);
        // N_FATAL("Failed to allocate memory for internal state");
        return nullptr;
    }

    internal_state->quit_flagged = false;

    @autoreleasepool {
        internal_state->app_delegate = [[ApplicationDelegate alloc] init];
        [NSApplication sharedApplication];

        if (!internal_state->app_delegate) {
            // N_FATAL("Failed to create application delegate");
            return nullptr;
        }
        [NSApp setDelegate:internal_state->app_delegate];

        if (![[NSRunningApplication currentApplication] isFinishedLaunching])
            [NSApp run];

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        NSRect frame = NSMakeRect(x, y, width, height);
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:style
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
        [window setTitle:[NSString stringWithUTF8String:application_name]];
        [window makeKeyAndOrderFront:nil];
    }

    window->data = internal_state;
    window->pump_messages = platform_window_pump_messages;

    platform_memory_destroy(memory);

    return window;
}

void platform_window_destroy(Window *window) {
    if (! window) {
        // N_FATAL("Window is null");
        return;
    }

    if (! window->data) {
        // N_FATAL("Window data is null");
        return;
    }

    InternalState *internal_state = window->data;

    @autoreleasepool {
        [NSApp terminate:nil];
    }

    Memory *memory = platform_memory_create();

    memory->used = true;

    memory->free(internal_state, false);
    memory->free(window, false);

    platform_memory_destroy(memory);
}

static bool1 platform_window_pump_messages(Window *window) {
    InternalState *internal_state = window->data;

    if (internal_state) {
        @autoreleasepool {

            NSEvent* event;

            for (;;) {
                event = [NSApp
                    nextEventMatchingMask:NSEventMaskAny
                                untilDate:[NSDate distantPast]
                                   inMode:NSDefaultRunLoopMode
                                  dequeue:YES];

                if (!event)
                    break;

                [NSApp sendEvent:event];
            }

        }

        return !internal_state->quit_flagged;
    }

    return true;
}

#endif
