//
// Created by Vinícius Ferreira Aguiar on 07/01/25.
//

#include "../platform.h"

#ifdef N_PLATFORM_APPLE

# include <core/logger.h>

# include <crt_externs.h>
# include <mach/mach_time.h>

# include <copyfile.h>
# include <errno.h>
# include <sys/stat.h>

# import <Cocoa/Cocoa.h>
# import <Foundation/Foundation.h>
# import <QuartzCore/CAMetalLayer.h>
# import <QuartzCore/QuartzCore.h>

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

bool1 platform_create(
    PlatformState *platform_state,
    const char *application_name,
    const int32 x,
    const int32 y,
    const int32 width,
    const int32 height)
{
    InternalState *internal_state = platform_allocate(sizeof(InternalState), false);
    if (!internal_state)
    {
        N_FATAL("Failed to allocate memory for platform state");
        return false;
    }

    platform_state->internal_state = internal_state;
    internal_state->quit_flagged = false;

    @autoreleasepool {
        internal_state->app_delegate = [[ApplicationDelegate alloc] init];
        [NSApplication sharedApplication];

        if (!internal_state->app_delegate) {
            N_FATAL("Failed to create application delegate");
            return false;
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

        return true;
    }
}

void platform_destroy(const PlatformState *platform_state) {
    InternalState *internal_state = platform_state->internal_state;

    if (internal_state) {
        @autoreleasepool {
            [NSApp terminate:nil];
        }
    }
}

bool1 platform_pump_messages(PlatformState *platform_state) {
    InternalState *internal_state = platform_state->internal_state;

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

void *platform_allocate(const uint64 size, const bool1 is_aligned)
{
    void *new_block = malloc(size);
    if (!new_block)
    {
        N_FATAL("Failed to allocate memory");
        return NULL;
    }

    return new_block;
}

void platform_free(void *block, const bool1 is_aligned) {
    if (block)
    {
        free(block);
    }
}

void *platform_zero_memory(void *block, const uint64 size) {
    return platform_set_memory(block, 0, size);
}

void *platform_copy_memory(void *destination, const void *source, const uint64 size) {
    return memcpy(destination, source, size);
}

void *platform_set_memory(void *destination, const int32 value, const uint64 size) {
    return memset(destination, value, size);
}

void platform_console_write(const char *message, const uint8 color) {
   const char *reset_color = "\033[0m";
   const char *color_strings[6] = {
       "\033[1;30m",
       "\033[1;34m",
       "\033[1;32m",
       "\033[1;33m",
       "\033[1;31m",
       "\033[0;41m",
   };

   fprintf(stdout, "%s%s%s", color_strings[color], message, reset_color);
}

void platform_console_error(const char *message, const uint8 color) {
   const char *reset_color = "\033[0m";
   const char *color_strings[6] = {
       "\033[1;30m",
       "\033[1;34m",
       "\033[1;32m",
       "\033[1;33m",
       "\033[1;31m",
       "\033[0;41m",
   };

   fprintf(stderr, "%s%s%s", color_strings[color], message, reset_color);
}

float64 platform_get_absolute_time(void) {
    mach_timebase_info_data_t clock_timebase;
    mach_timebase_info(&clock_timebase);

    uint64 mach_absolute = mach_absolute_time();

    float64 nanos = (float64) ((float64) mach_absolute * (float64) clock_timebase.numer) / (float64) clock_timebase.denom;
    return nanos / 1.0e9; // Convert to seconds
}

void platform_sleep(uint64 milliseconds) {
# if _POSIX_C_SOURCE >= 199309L
    struct timespec time;

    time.tv_sec = milliseconds / 1000;
    time.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    nanosleep(&time, 0);
# else
    if (milliseconds >= 1000)
    {
        sleep((uint32)(milliseconds / 1000));
    }

    usleep(milliseconds % 1000 * 1000);
# endif
}

#endif