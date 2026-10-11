/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/console_journey.c
 * PURPOSE: Bounded interactive serial protocol, independent of the QMP codec.
 *
 * A line is never submitted by a host shell. Each printable byte is sent to
 * the guest console, then the exact echoed byte is observed before the next
 * byte is sent. CR is the only submit character. On error there is no retry.
 * The protocol never turns a prompt into evidence that a disk is durable.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/console_journey.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_VM_JOURNEY_PROMPT "umicom> "

typedef struct UmiVmConsoleStream {
    const UmiVmConsoleJourneyTransport *io;
    UmiVmConsoleJourneyReport *report;
    char *buffer;
    size_t length;
    uint64_t previousClock;
    size_t stationaryClockSamples;
} UmiVmConsoleStream;

const char *UmiVmConsoleJourneyStatusName(UmiVmConsoleJourneyStatus status)
{
    switch (status) {
        case UMI_VM_CONSOLE_JOURNEY_OK: return "ok";
        case UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT: return "invalid-argument";
        case UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE: return "transport-failure";
        case UMI_VM_CONSOLE_JOURNEY_TIMEOUT: return "timeout";
        case UMI_VM_CONSOLE_JOURNEY_CLOCK_ROLLBACK: return "clock-rollback";
        case UMI_VM_CONSOLE_JOURNEY_CLOCK_STALLED: return "clock-stalled";
        case UMI_VM_CONSOLE_JOURNEY_OUTPUT_LIMIT: return "output-limit";
        case UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE: return "protocol-failure";
        case UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE: return "guest-failure";
        case UMI_VM_CONSOLE_JOURNEY_EXPECTATION_MISSING: return "expectation-missing";
        default: return "unknown-status";
    }
}

static int UmicomJourneyTextValid(const char *text, size_t limit)
{
    size_t count = 0U;
    if (!text) return 0;
    for (; count < limit && text[count]; ++count) {
        const unsigned char byte = (unsigned char)text[count];
        if (byte < 32U || byte > 126U) return 0;
    }
    return count != 0U && count < limit;
}

static UmiVmConsoleJourneyStatus UmicomJourneyClock(UmiVmConsoleStream *stream,
    uint64_t start, uint64_t timeout)
{
    const uint64_t now = stream->io->clockMilliseconds(stream->io->context);
    if (now < stream->previousClock || now < start)
        return UMI_VM_CONSOLE_JOURNEY_CLOCK_ROLLBACK;
    if (now == stream->previousClock) {
        /* A broken clock callback must not create an infinite read loop.
         * Real monotonic milliseconds need not advance on every poll. */
        if (++stream->stationaryClockSamples > 20000U)
            return UMI_VM_CONSOLE_JOURNEY_CLOCK_STALLED;
    } else stream->stationaryClockSamples = 0U;
    stream->previousClock = now;
    return now - start >= timeout ? UMI_VM_CONSOLE_JOURNEY_TIMEOUT : UMI_VM_CONSOLE_JOURNEY_OK;
}

static int UmicomJourneyContains(const UmiVmConsoleStream *stream,
    size_t from, const char *needle)
{
    const size_t target = strlen(needle);
    if (from > stream->length || target > stream->length - from) return 0;
    for (size_t at = from; at <= stream->length - target; ++at) {
        if (memcmp(stream->buffer + at, needle, target) == 0) return 1;
    }
    return 0;
}

/* Fail even when the guest subsequently displays a normal prompt. The
 * complete boot/command buffer is inspected; an earlier failure marker must
 * not be hidden by the suffix check. */
static UmiVmConsoleJourneyStatus UmicomJourneyReceive(UmiVmConsoleStream *stream,
    uint64_t start, uint64_t timeout)
{
    unsigned char bytes[256];
    size_t received = 0U;
    UmiVmConsoleJourneyStatus status = UmicomJourneyClock(stream, start, timeout);
    if (status != UMI_VM_CONSOLE_JOURNEY_OK) return status;
    if (!stream->io->read(stream->io->context, bytes, sizeof bytes, &received))
        return UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE;
    if (received > sizeof bytes) return UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE;
    if (received) {
        if (received > UMI_VM_CONSOLE_JOURNEY_OUTPUT_CAPACITY - stream->length - 1U)
            return UMI_VM_CONSOLE_JOURNEY_OUTPUT_LIMIT;
        memcpy(stream->buffer + stream->length, bytes, received);
        stream->length += received;
        stream->buffer[stream->length] = '\0';
        stream->report->receivedBytes += received;
        if (UmicomJourneyContains(stream, 0U, "UMICOM_KERNEL_FAIL") ||
            UmicomJourneyContains(stream, 0U, "unexpected-trap") ||
            UmicomJourneyContains(stream, 0U, "PANIC"))
            return UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE;
    } else if (stream->io->idle) {
        stream->io->idle(stream->io->context);
    }
    return UmicomJourneyClock(stream, start, timeout);
}

static UmiVmConsoleJourneyStatus UmicomJourneyWait(UmiVmConsoleStream *stream,
    size_t after, const char *expected, int exactEcho, uint64_t start, uint64_t timeout)
{
    const size_t expectedLength = strlen(expected);
    for (;;) {
        if (stream->length >= after + expectedLength) {
            if (exactEcho) {
                if (memcmp(stream->buffer + after, expected, expectedLength) == 0)
                    return UMI_VM_CONSOLE_JOURNEY_OK;
                return UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE;
            }
            if (memcmp(stream->buffer + stream->length - expectedLength,
                    expected, expectedLength) == 0)
                return UMI_VM_CONSOLE_JOURNEY_OK;
        }
        UmiVmConsoleJourneyStatus status = UmicomJourneyReceive(stream, start, timeout);
        if (status != UMI_VM_CONSOLE_JOURNEY_OK) return status;
    }
}

static UmiVmConsoleJourneyStatus UmicomJourneySend(UmiVmConsoleStream *stream,
    const void *bytes, size_t length)
{
    if (!stream->io->write(stream->io->context, bytes, length))
        return UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE;
    stream->report->transmittedBytes += length;
    return UMI_VM_CONSOLE_JOURNEY_OK;
}

UmiVmConsoleJourneyStatus UmiVmConsoleJourneyRun(
    const UmiVmConsoleJourneyTransport *transport,
    const UmiVmConsoleJourneyStep *steps,
    size_t count,
    uint64_t timeoutMilliseconds,
    UmiVmConsoleJourneyReport *report)
{
    if (!report) return UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    report->status = UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
    if (!transport || !transport->write || !transport->read ||
        !transport->clockMilliseconds || !steps || !count ||
        count > UMI_VM_CONSOLE_JOURNEY_COMMAND_LIMIT ||
        timeoutMilliseconds < 100U || timeoutMilliseconds > 300000U)
        return report->status;
    for (size_t i = 0U; i < count; ++i) {
        if (!UmicomJourneyTextValid(steps[i].command,
                UMI_VM_CONSOLE_JOURNEY_COMMAND_CAPACITY) ||
            !UmicomJourneyTextValid(steps[i].expectedText,
                UMI_VM_CONSOLE_JOURNEY_EXPECT_CAPACITY))
            return report->status;
    }
    char *buffer = calloc(UMI_VM_CONSOLE_JOURNEY_OUTPUT_CAPACITY, 1U);
    if (!buffer) {
        report->status = UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE;
        return report->status;
    }
    UmiVmConsoleStream stream = {transport, report, buffer, 0U,
        transport->clockMilliseconds(transport->context), 0U};
    uint64_t start = stream.previousClock;
    UmiVmConsoleJourneyStatus status = UmicomJourneyWait(&stream, 0U,
        UMI_VM_JOURNEY_PROMPT, 0, start, timeoutMilliseconds);
    if (status != UMI_VM_CONSOLE_JOURNEY_OK) goto finish;
    for (size_t step = 0U; step < count; ++step) {
        const char *command = steps[step].command;
        report->failedStep = step;
        stream.length = 0U;
        buffer[0] = '\0';
        start = transport->clockMilliseconds(transport->context);
        if (start < stream.previousClock) {
            status = UMI_VM_CONSOLE_JOURNEY_CLOCK_ROLLBACK;
            goto finish;
        }
        for (size_t i = 0U; command[i]; ++i) {
            const size_t before = stream.length;
            status = UmicomJourneySend(&stream, command + i, 1U);
            if (status != UMI_VM_CONSOLE_JOURNEY_OK) goto finish;
            /* Echo is exactly one byte, not the remaining command suffix. */
            const char echo[2] = {command[i], '\0'};
            status = UmicomJourneyWait(&stream, before, echo,
                1, start, timeoutMilliseconds);
            if (status != UMI_VM_CONSOLE_JOURNEY_OK) goto finish;
        }
        const unsigned char carriageReturn = 13U;
        const size_t after = stream.length;
        status = UmicomJourneySend(&stream, &carriageReturn, 1U);
        if (status != UMI_VM_CONSOLE_JOURNEY_OK) goto finish;
        status = UmicomJourneyWait(&stream, after, UMI_VM_JOURNEY_PROMPT, 0,
            start, timeoutMilliseconds);
        if (status != UMI_VM_CONSOLE_JOURNEY_OK) goto finish;
        if (UmicomJourneyContains(&stream, after,
                "unknown command or incorrect arguments")) {
            status = UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE;
            goto finish;
        }
        if (!UmicomJourneyContains(&stream, after, steps[step].expectedText)) {
            status = UMI_VM_CONSOLE_JOURNEY_EXPECTATION_MISSING;
            goto finish;
        }
        ++report->completedSteps;
    }
    /* A successful run has no failed index. Use the count sentinel rather
     * than leaving the index of the final successful step in that field. */
    report->failedStep = count;
finish:
    report->status = status;
    (void)snprintf(report->detail, sizeof report->detail,
        "console-journey=%s steps=%zu/%zu received=%zu sent=%zu",
        UmiVmConsoleJourneyStatusName(status), report->completedSteps,
        count, report->receivedBytes, report->transmittedBytes);
    free(buffer);
    return status;
}
