/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/program_console_worker.c
 * PURPOSE: Keep every pipe operation on the dedicated console worker and bound input waits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "program_console_internal.h"
#include "umicom/platform/clock.h"
#include <string.h>

static uint64_t ConsoleMilliseconds(UmiClock *clock)
{
    return clock->monotonic_nanoseconds(clock) / 1000000U;
}
UmiStatus UmiProgramConsoleRun(UmiProgramConsole *console)
{
    if (console == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(console->mutex);
    if (console->entered)
    {
        (void)umi_mutex_unlock(console->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    console->entered = true;
    bool stopped = console->snapshot.stop_requested;
    (void)umi_mutex_unlock(console->mutex);
    UmiStatus status = stopped ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
    UmiProcessChannel *channel = NULL;
    const UmiEnvironmentVariable *environment = NULL;
    size_t environment_count = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = ConsoleMilliseconds(&clock);
    if (status == UMI_STATUS_OK)
        status =
            UmiProcessEnvironmentPlanRead(console->environment, &environment, &environment_count);
    if (status == UMI_STATUS_OK)
        status = UmiProcessChannelOpenProgram(&console->command, environment, environment_count,
                                              &channel);
    if (status == UMI_STATUS_OK)
    {
        (void)umi_mutex_lock(console->mutex);
        console->snapshot.started = true;
        (void)umi_mutex_unlock(console->mutex);
    }
    bool input_closed = false;
    /* Service output between input frames. A program that never reads stdin
     * cannot hold a GUI indefinitely; a timed-out write is not retried because
     * a prefix may already have reached the child. */
    while (status == UMI_STATUS_OK)
    {
        ConsoleInput frame = {0};
        bool end_input;
        (void)umi_mutex_lock(console->mutex);
        stopped = console->snapshot.stop_requested;
        end_input = console->snapshot.input_ended;
        if (!stopped && console->count != 0U)
        {
            frame = console->queue[console->head];
            memset(&console->queue[console->head], 0, sizeof frame);
            console->head = (console->head + 1U) % UMI_PROGRAM_CONSOLE_QUEUE_CAPACITY;
            --console->count;
            console->snapshot.queued_frames = console->count;
        }
        (void)umi_mutex_unlock(console->mutex);
        if (stopped)
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (console->timeout_ms != 0U &&
            ConsoleMilliseconds(&clock) - started >= console->timeout_ms)
        {
            status = UMI_STATUS_TIMEOUT;
            break;
        }
        UmiProcessChannelSnapshot child = {0};
        status = UmiProcessChannelPoll(channel, &child);
        if (status != UMI_STATUS_OK)
            break;
        ConsolePublishChannel(console, &child);
        if (frame.length != 0U && child.running)
        {
            status = UmiProcessChannelWrite(channel, frame.bytes, frame.length, 250U);
            if (status != UMI_STATUS_OK)
                break;
            (void)umi_mutex_lock(console->mutex);
            uint64_t left = UINT64_MAX - console->snapshot.input_bytes_sent;
            console->snapshot.input_bytes_sent +=
                frame.length > left ? left : (uint64_t)frame.length;
            (void)umi_mutex_unlock(console->mutex);
        }
        memset(&frame, 0, sizeof frame);
        if (end_input && !input_closed)
        {
            (void)umi_mutex_lock(console->mutex);
            bool empty = console->count == 0U;
            (void)umi_mutex_unlock(console->mutex);
            if (empty)
            {
                status = UmiProcessChannelCloseInput(channel);
                input_closed = status == UMI_STATUS_OK;
            }
        }
        if (status != UMI_STATUS_OK)
            break;
        char output[4096];
        size_t received = 0U;
        UmiStatus read_status =
            UmiProcessChannelRead(channel, output, sizeof output, &received, 10U);
        if (read_status == UMI_STATUS_OK && received != 0U)
            ConsoleOutput(console, output, received);
        else if (read_status != UMI_STATUS_OK && read_status != UMI_STATUS_TIMEOUT)
        {
            status = read_status;
            break;
        }
        if (!child.running && received == 0U)
            break;
        /* A child may close stdout while still reading stdin or writing stderr. */
        if (read_status == UMI_STATUS_OK && received == 0U && child.running)
            umi_thread_sleep_ms(10U);
    }
    if (channel != NULL)
    {
        if (status != UMI_STATUS_OK)
            (void)UmiProcessChannelTerminate(channel);
        UmiProcessChannelSnapshot child = {0};
        if (UmiProcessChannelPoll(channel, &child) == UMI_STATUS_OK)
            ConsolePublishChannel(console, &child);
        UmiProcessChannelDestroy(channel);
    }
    umi_clock_dispose(&clock);
    /* Completion is published only after native ownership has been released.
     * The caller must still join the thread before freeing its controller. */
    (void)umi_mutex_lock(console->mutex);
    memset(console->queue, 0, sizeof console->queue);
    console->count = 0U;
    console->snapshot.queued_frames = 0U;
    console->snapshot.status = status;
    console->snapshot.completed = true;
    (void)umi_mutex_unlock(console->mutex);
    return status;
}
