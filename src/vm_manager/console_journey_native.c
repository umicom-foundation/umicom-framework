/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/console_journey_native.c
 * PURPOSE: Adapt bounded serial qualification to the existing owned QMP VM.
 *
 * The adapter never spawns QEMU, never sends raw monitor syntax, never selects
 * a host disk and never edits the Kernel. UmiVmControl continues to own QMP
 * frame validation, correlated replies, child lifetime and ring-buffer I/O.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#include "umicom/vm_manager/console_journey.h"
#include "internal.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <time.h>
#endif

typedef struct UmicomJourneyNativeState {
    UmiVmSession *session;
    /* QMP reads up to 2048 bytes while the protocol intentionally consumes
     * bounded chunks. Keep unconsumed serial bytes until the next poll. */
    unsigned char pending[UMI_VM_CONSOLE_CHUNK];
    size_t offset;
    size_t length;
} UmicomJourneyNativeState;

static int UmicomJourneyQmpWrite(void *context, const void *bytes, size_t length)
{
    UmicomJourneyNativeState *native = context;
    UmiVmReport report = {0};
    return UmiVmControl(native->session, UMI_VM_CONSOLE_WRITE,
        bytes, length, NULL, 0U, NULL, &report) == UMI_STATUS_OK;
}

static int UmicomJourneyQmpRead(void *context, void *bytes, size_t capacity,
    size_t *outLength)
{
    UmicomJourneyNativeState *native = context;
    if (!native || !outLength || !bytes || !capacity) return 0;
    if (native->offset == native->length) {
        native->offset = 0U;
        native->length = 0U;
        UmiVmReport report = {0};
        if (UmiVmControl(native->session, UMI_VM_CONSOLE_READ,
                NULL, 0U, native->pending, sizeof native->pending,
                &native->length, &report) != UMI_STATUS_OK ||
            native->length > sizeof native->pending) return 0;
    }
    size_t available = native->length - native->offset;
    if (available > capacity) available = capacity;
    if (available) {
        memcpy(bytes, native->pending + native->offset, available);
        native->offset += available;
    }
    *outLength = available;
    return 1;
}

static uint64_t UmicomJourneyQmpClock(void *context)
{
    (void)context;
    return VmClock();
}

static void UmicomJourneyQmpIdle(void *context)
{
    (void)context;
#ifdef _WIN32
    Sleep(1U);
#else
    const struct timespec pause = {0, 1000000L};
    (void)nanosleep(&pause, NULL);
#endif
}

UmiVmConsoleJourneyStatus UmiVmConsoleJourneyRunNative(
    UmiVmSession *session, const UmiVmConsoleJourneyStep *steps,
    size_t count, uint64_t timeoutMilliseconds, UmiVmConsoleJourneyReport *report)
{
    if (!session) {
        if (report) {
            memset(report, 0, sizeof *report);
            report->status = UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
        }
        return UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT;
    }
    UmicomJourneyNativeState native = {0};
    native.session = session;
    UmiVmConsoleJourneyTransport transport = {
        &native, UmicomJourneyQmpWrite, UmicomJourneyQmpRead,
        UmicomJourneyQmpClock, UmicomJourneyQmpIdle
    };
    return UmiVmConsoleJourneyRun(&transport, steps, count,
        timeoutMilliseconds, report);
}
