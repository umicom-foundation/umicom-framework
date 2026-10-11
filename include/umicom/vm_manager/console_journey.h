/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vm_manager/console_journey.h
 *
 * PURPOSE:
 *   Qualify an interactive guest console through a bounded, transport-neutral
 *   read/write contract. A real QEMU adapter uses the Framework VM Manager's
 *   existing owned process and QMP channel; deterministic native tests use a
 *   memory-backed transport without starting a guest.
 *
 * ARCHITECTURE:
 *   The Kernel has no dependency on this user-space API. A console transcript
 *   is evidence of the named commands only, not a claim of storage durability,
 *   clean poweroff or a complete operating-system release.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VM_MANAGER_CONSOLE_JOURNEY_H
#define UMICOM_VM_MANAGER_CONSOLE_JOURNEY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_VM_CONSOLE_JOURNEY_COMMAND_CAPACITY 128U
#define UMI_VM_CONSOLE_JOURNEY_EXPECT_CAPACITY 128U
#define UMI_VM_CONSOLE_JOURNEY_COMMAND_LIMIT 8U
#define UMI_VM_CONSOLE_JOURNEY_OUTPUT_CAPACITY 32768U

typedef enum UmiVmConsoleJourneyStatus {
    UMI_VM_CONSOLE_JOURNEY_OK = 0,
    UMI_VM_CONSOLE_JOURNEY_INVALID_ARGUMENT,
    UMI_VM_CONSOLE_JOURNEY_TRANSPORT_FAILURE,
    UMI_VM_CONSOLE_JOURNEY_TIMEOUT,
    UMI_VM_CONSOLE_JOURNEY_CLOCK_ROLLBACK,
    UMI_VM_CONSOLE_JOURNEY_CLOCK_STALLED,
    UMI_VM_CONSOLE_JOURNEY_OUTPUT_LIMIT,
    UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE,
    UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE,
    UMI_VM_CONSOLE_JOURNEY_EXPECTATION_MISSING
} UmiVmConsoleJourneyStatus;

typedef struct UmiVmConsoleJourneyStep {
    /* Printable ASCII, without CR, LF, quote expansion or a host shell. */
    const char *command;
    /* A required literal in the command's response, not in the boot banner. */
    const char *expectedText;
} UmiVmConsoleJourneyStep;

typedef struct UmiVmConsoleJourneyTransport {
    void *context;
    /* Return nonzero only when the complete write was accepted. */
    int (*write)(void *context, const void *bytes, size_t length);
    /* Return nonzero on success; a zero-byte read is not an EOF assertion. */
    int (*read)(void *context, void *buffer, size_t capacity, size_t *outLength);
    /* Milliseconds since any arbitrary monotonic origin. */
    uint64_t (*clockMilliseconds)(void *context);
    /* Optional bounded idle callback. It may yield but must not mutate state. */
    void (*idle)(void *context);
} UmiVmConsoleJourneyTransport;

typedef struct UmiVmConsoleJourneyReport {
    UmiVmConsoleJourneyStatus status;
    size_t completedSteps;
    size_t failedStep;
    size_t receivedBytes;
    size_t transmittedBytes;
    char detail[160];
} UmiVmConsoleJourneyReport;

/* Each prompt and command has its own inclusive elapsed-time bound. An invalid
 * plan is refused before any read or write. No command is retried after an
 * uncertain transport write, timeout, output overflow or guest failure. */
UmiVmConsoleJourneyStatus UmiVmConsoleJourneyRun(
    const UmiVmConsoleJourneyTransport *transport,
    const UmiVmConsoleJourneyStep *steps,
    size_t count,
    uint64_t timeoutMilliseconds,
    UmiVmConsoleJourneyReport *report);

const char *UmiVmConsoleJourneyStatusName(UmiVmConsoleJourneyStatus status);

/* The native adapter only borrows a live VM Manager session. It does not boot,
 * stop or destroy QEMU; that lifecycle belongs to the caller. */
typedef struct UmiVmSession UmiVmSession;
UmiVmConsoleJourneyStatus UmiVmConsoleJourneyRunNative(
    UmiVmSession *session,
    const UmiVmConsoleJourneyStep *steps,
    size_t count,
    uint64_t timeoutMilliseconds,
    UmiVmConsoleJourneyReport *report);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_VM_MANAGER_CONSOLE_JOURNEY_H */
