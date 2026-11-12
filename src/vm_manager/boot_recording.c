/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_recording.c
 *
 * PURPOSE:
 *   Reserve a new serial-recording directory through the shared checked filesystem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "boot_internal.h"
#include "umicom/platform/output_file.h"

UmiStatus VmBootRecordingValidate(const char *directory)
{
    if (!directory) return UMI_STATUS_OK;
    if (!directory[0] || strlen(directory) >= UMI_VM_PATH || !VmPath(directory, 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0; directory[index]; ++index) {
        unsigned char byte = (unsigned char)directory[index];
        if (byte < 32U || byte == 127U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Validate the derived filename during pure planning, before a start can
     * reserve its directory. Reuse the raw writer's native path contract. */
    /* A valid directory can already fill the VM path field. Appending the
     * diagnostic filename therefore needs the setup service's output capacity.
     * Keep the former declaration for review; planning must never write beyond
     * its local storage while validating a derived filename. */
#if 0
    char diagnostics[UMI_VM_PATH];
#endif
    char diagnostics[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status = UmiSetupPathJoin(directory, "qemu-errors.log", diagnostics);
    return status == UMI_STATUS_OK ? UmiOutputFileValidatePath(diagnostics) : status;
}

/* Review is read-only. A successful review does not reserve a directory; start
 * must win exclusive creation as well, so a second run cannot replace evidence. */
UmiStatus VmBootRecordingReview(const UmiVmBootPlan *plan)
{
    if (!plan->recordingDirectory[0]) return UMI_STATUS_OK;
    UmiStatus status = UmiSetupFileCheck(plan->recordingDirectory, 1, NULL);
    if (status != UMI_STATUS_NOT_FOUND)
        return status == UMI_STATUS_OK ? UMI_STATUS_ALREADY_EXISTS : status;
    return UmiSetupFileParentCheck(plan->recordingDirectory, NULL);
}

UmiStatus VmBootRecordingBegin(const UmiVmBootPlan *plan, UmiVmReport *report)
{
    if (!plan->recordingDirectory[0]) return UMI_STATUS_OK;
    UmiStatus status = UmiSetupDirectoryCreate(plan->recordingDirectory, NULL);
    if (status != UMI_STATUS_OK) return status;
    report->outputCreated = 1;
    /* Keep partial output on every failure. The review identity binds the
     * selected inputs and destination; it does not certify emulator behavior. */
    char record[512];
    int size = snprintf(record, sizeof record,
        "UMICOM_VM_BOOT_RECORDING\t1\nreview\t%s\nserial\tserial.log\n"
/* The recording manifest now names independently owned host error output; the original serial-only field line is retained. */
#if 0
        "guest-output\tqemu-chardev\nboot-qualified\tno\n",
#endif
        "diagnostics\tqemu-errors.log\nguest-output\tqemu-chardev\nboot-qualified\tno\n",
        report->fingerprint);
    if (size < 0 || (size_t)size >= sizeof record) return UMI_STATUS_CAPACITY_EXCEEDED;
    return VmWriteJoined(plan->recordingDirectory, "recording.umi", record, (size_t)size);
}
