/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/kernel_disk_qualification.c
 *
 * PURPOSE:
 *   Exercise the real Umicom Kernel FAT16 Stage/Finish protocol through the
 *   existing QMP VM Manager and independent C23 full-image verifier.
 *
 * OWNERSHIP AND SAFETY:
 *   The caller selects an exact, known synthetic source and a NEW output
 *   directory. Only the validated copy is ever attached writable to QEMU;
 *   normal BootPlanCreate continues using snapshot=on. The copy is retained
 *   as evidence. Existing Framework process ownership and QMP controls are
 *   used instead of adding a second process launcher or a shell script.
 *
 * A successful Stage is stopped AFTER completed flush/readback (not during
 * a sector write). Completion is judged only after both the independent
 * read-only whole-image verifier and a fresh read-only guest have passed.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "boot_internal.h"
#include "kernel_disk_plan.h"
#include "umicom/vm_manager/kernel_disk_qualification.h"
#include "umicom/setup_centre/files.h"
#include "umicom/platform/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KERNEL_FAT16_FIXTURE_BYTES UINT64_C(8388608)
#define KERNEL_FAT16_FIXTURE_SHA256 "bd866d6ae337527f3a8b4609f31969d60185e025f8525726f573d905a2f50938"
#define KERNEL_FAT16_SCRATCH_NAME "umicom-kernel-owned-fat16.raw"

static int KernelCopyText(char *destination, size_t capacity, const char *source)
{
    if (!destination || !source || !source[0]) return 0;
    const size_t length = strlen(source);
    if (length >= capacity) return 0;
    memcpy(destination, source, length + 1U);
    return 1;
}

UmiStatus UmiVmKernelDiskRequestValidate(const UmiVmKernelDiskRequest *request)
{
    if (!request || !request->qemuExecutable || !request->firmware ||
        !request->workingDirectory || !request->fixture ||
        !request->outputDirectory || !request->verifier ||
        (request->exercise != UMI_VM_KERNEL_DISK_STAGED &&
         request->exercise != UMI_VM_KERNEL_DISK_FINISHED) ||
        request->blockSlot > 7U ||
        request->commandTimeoutMilliseconds < UINT64_C(100) ||
        request->commandTimeoutMilliseconds > UINT64_C(180000))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *const paths[] = {request->qemuExecutable, request->firmware,
        request->workingDirectory, request->fixture, request->outputDirectory,
        request->verifier};
    for (size_t i = 0U; i < sizeof paths / sizeof paths[0]; ++i) {
        if (!paths[i][0] || strlen(paths[i]) >= UMI_VM_PATH ||
            !VmPath(paths[i], 0)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Explicit scratch ownership cannot equal a source, executable, or
     * application work directory. A sibling path is fine and intentional. */
    if (strcmp(request->fixture, request->outputDirectory) == 0 ||
        strcmp(request->workingDirectory, request->outputDirectory) == 0 ||
        strcmp(request->firmware, request->outputDirectory) == 0 ||
        strcmp(request->qemuExecutable, request->outputDirectory) == 0 ||
        strcmp(request->verifier, request->outputDirectory) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static void KernelFailure(UmiVmKernelDiskReport *report, UmiStatus status,
    const char *message)
{
    report->status = status;
    (void)snprintf(report->details, sizeof report->details, "%s", message);
}

static UmiStatus KernelScratchCreate(const UmiVmKernelDiskRequest *request,
    UmiVmKernelDiskReport *report)
{
    /* Permission errors, symlinks or malformed paths are NOT evidence that
     * the scratch directory is absent. Only NOT_FOUND permits creation. */
    const UmiStatus outputInspection = UmiSetupFileCheck(
        request->outputDirectory, 1, NULL);
    if (outputInspection != UMI_STATUS_NOT_FOUND) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "The scratch directory is present or cannot safely be inspected; use a NEW path.");
        return report->status;
    }
    UmiStatus status = UmiSetupFileCheck(request->fixture, 0, NULL);
    if (status != UMI_STATUS_OK) {
        KernelFailure(report, status, "Cannot read the original regular synthetic fixture.");
        return status;
    }
    uint64_t sourceBytes = 0U;
    status = UmiSetupFileDigest(request->fixture, report->fixtureHash, &sourceBytes, NULL);
    if (status != UMI_STATUS_OK || sourceBytes != KERNEL_FAT16_FIXTURE_BYTES ||
        strcmp(report->fixtureHash, KERNEL_FAT16_FIXTURE_SHA256) != 0) {
        KernelFailure(report, UMI_STATUS_INVALID_ARGUMENT,
            "The source is not the exact, qualified 8-MiB synthetic FAT16 fixture.");
        return report->status;
    }
    status = UmiSetupDirectoryCreate(request->outputDirectory, NULL);
    if (status != UMI_STATUS_OK) {
        KernelFailure(report, status, "Could not create the new scratch directory.");
        return status;
    }
    char path[UMI_SETUP_PATH_CAPACITY];
    status = UmiSetupPathJoin(request->outputDirectory, KERNEL_FAT16_SCRATCH_NAME, path);
    if (status != UMI_STATUS_OK || strlen(path) >= sizeof report->disk) {
        KernelFailure(report, UMI_STATUS_CAPACITY_EXCEEDED,
            "The scratch disk path exceeds the VM Manager path limit.");
        return report->status;
    }
    if (UmiSetupFileCheck(path, 0, NULL) != UMI_STATUS_NOT_FOUND) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "The scratch disk exists or cannot be safely inspected; refusing replacement.");
        return report->status;
    }
    status = UmiSetupFileCopyChecked(request->fixture, path,
        report->fixtureHash, sourceBytes, NULL);
    if (status != UMI_STATUS_OK) {
        KernelFailure(report, status, "Checked copy of the synthetic disk failed.");
        return status;
    }
    char copiedHash[65];
    uint64_t copiedBytes = 0U;
    status = UmiSetupFileSingleLink(path, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiSetupFileDigest(path, copiedHash, &copiedBytes, NULL);
    if (status != UMI_STATUS_OK || copiedBytes != sourceBytes ||
        strcmp(copiedHash, report->fixtureHash) != 0) {
        KernelFailure(report, UMI_STATUS_IO_ERROR,
            "Scratch image does not match the immutable fixture after copying.");
        return report->status;
    }
    (void)KernelCopyText(report->disk, sizeof report->disk, path);
    return UMI_STATUS_OK;
}

/* Reuse the normal BootPlanCreate and its reviewed security options. This
 * private mutation accepts only its exact QEMU-generated argument vector;
 * ordinary VM sessions continue to use their unmodified snapshot-on mode. */
static UmiStatus KernelCreateOwnedPlan(const UmiVmKernelDiskRequest *request,
    const char *disk, int readOnly, UmiVmBootPlan **out)
{
    *out = NULL;
    UmiVmBootRequest boot;
    UmiStatus status = UmiVmBootRequestInit(&boot, UMI_VM_BOOT_UMICOM_KERNEL);
    if (status != UMI_STATUS_OK) return status;
    if (!KernelCopyText(boot.executable, sizeof boot.executable, request->qemuExecutable) ||
        !KernelCopyText(boot.workingDirectory, sizeof boot.workingDirectory,
            request->workingDirectory) ||
        !KernelCopyText(boot.firmware, sizeof boot.firmware, request->firmware) ||
        !KernelCopyText(boot.disk, sizeof boot.disk, disk)) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiVmBootPlanCreate(&boot, out);
    if (status != UMI_STATUS_OK) return status;
    if (!UmiVmKernelDiskPatchArguments((*out)->arguments, &(*out)->argumentCount,
            UMI_CHANNEL_MAX_ARGUMENTS, VM_BOOT_ARGUMENT_BYTES, readOnly)) {
        UmiVmBootPlanDestroy(*out);
        *out = NULL;
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus KernelRunGuest(const UmiVmKernelDiskRequest *request,
    UmiVmKernelDiskReport *result, int readOnly)
{
    UmiVmBootPlan *plan = NULL;
    UmiVmSession *session = NULL;
    UmiVmReport vm = {0};
    UmiStatus status = KernelCreateOwnedPlan(request, result->disk, readOnly, &plan);
    if (status == UMI_STATUS_OK) status = UmiVmBootReview(plan, &vm);
    if (status == UMI_STATUS_OK) status = UmiVmBootStart(plan, vm.fingerprint, &session, &vm);
    if (status == UMI_STATUS_OK)
        status = UmiVmControl(session, UMI_VM_RESUME, NULL, 0U,
            NULL, 0U, NULL, &vm);
    if (status != UMI_STATUS_OK) {
        KernelFailure(result, status, "Reviewed QEMU boot or resume failed.");
    } else {
        char disks[128], open[128], mount[128];
        const int n1 = snprintf(disks, sizeof disks, "block.slot=%u base=", request->blockSlot);
        const int n2 = snprintf(open, sizeof open, "fatcommitopen %u 0", request->blockSlot);
        const int n3 = snprintf(mount, sizeof mount, "mountdisk %u 0", request->blockSlot);
        if (n1 <= 0 || (size_t)n1 >= sizeof disks ||
            n2 <= 0 || (size_t)n2 >= sizeof open ||
            n3 <= 0 || (size_t)n3 >= sizeof mount) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            KernelFailure(result, status, "Invalid QEMU block slot formatting.");
        } else {
            UmiVmConsoleJourneyStep writer[] = {
                {"disks", disks},
                {open, "fat.commit.open=ok"},
                {"fatstage /FRAG.BIN 511 \"Umicom ordered update\"", "fat.commit.stage=ok"},
                {"fatcommitinfo", "fat.commit-state=staged"},
                {"fatcommit", "fat.commit.finish=ok"},
                {"fatcommitclose", "fat.commit.release=ok"}
            };
            UmiVmConsoleJourneyStep reader[] = {
                {"disks", disks},
                {mount, request->exercise == UMI_VM_KERNEL_DISK_STAGED ?
                    "disk.media=unclean-volume" : "disk.mount=ok"},
                {"mountinfo", "disk.mount-state=mounted"}
            };
            UmiVmConsoleJourneyReport journey = {0};
            const size_t count = readOnly ?
                (request->exercise == UMI_VM_KERNEL_DISK_STAGED ? 2U : 3U) :
                (request->exercise == UMI_VM_KERNEL_DISK_STAGED ? 4U : 6U);
            UmiVmConsoleJourneyStatus consoleStatus = UmiVmConsoleJourneyRunNative(
                session, readOnly ? reader : writer, count,
                request->commandTimeoutMilliseconds, &journey);
            result->consoleStatus = consoleStatus;
            if (consoleStatus != UMI_VM_CONSOLE_JOURNEY_OK) {
                status = UMI_STATUS_INVALID_STATE;
                (void)snprintf(result->details, sizeof result->details,
                    "Guest console refused at step %zu: %s",
                    journey.failedStep, journey.detail);
                result->status = status;
            } else {
                result->bootsCompleted++;
                if (readOnly) result->readerObserved = 1;
            }
        }
    }
    /* The runner owns exactly this child. For staged mode this deliberately
     * simulates loss of the running guest after durable Stage; not an IRQ cut.
     * Completed mode first finishes the clean barriers and closes the lease. */
    if (session) {
        UmiVmReport stop = {0};
        const UmiStatus stopped = UmiVmForceStop(session, &stop);
        if (stopped != UMI_STATUS_OK && status == UMI_STATUS_OK) {
            status = stopped;
            KernelFailure(result, status, "Could not stop the owned QEMU session.");
        }
    }
    UmiVmSessionDestroy(session);
    UmiVmBootPlanDestroy(plan);
    return status;
}

static UmiStatus KernelVerify(const UmiVmKernelDiskRequest *request,
    UmiVmKernelDiskReport *report)
{
    const char *const arguments[3] = {request->fixture, report->disk,
        request->exercise == UMI_VM_KERNEL_DISK_STAGED ? "stage" : "finish"};
    UmiProcessRequest process = {0};
    UmiProcessResult result = {0};
    process.program = request->verifier;
    process.arguments = arguments;
    process.argument_count = 3U;
    process.working_directory = request->workingDirectory;
    process.capture_stdout = 1;
    process.capture_stderr = 1;
    process.timeout_ms = 60000U;
    process.poll_interval_ms = 25U;
    process.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiStatus status = umi_process_execute(&process, &result);
    if (status != UMI_STATUS_OK || !result.launched || result.exit_code != 0 ||
        result.timed_out || result.output_truncated ||
        !strstr(result.output, "\"classification\":\"verified\"") ||
        !strstr(result.output, "\"full_image_compared\":true") ||
        !strstr(result.output, "\"repair_performed\":false")) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "The independent C23 full-image verifier did not confirm the stopped guest disk.");
        return report->status;
    }
    uint64_t bytes = 0U;
    status = UmiSetupFileDigest(report->disk, report->diskAfterWriterHash, &bytes, NULL);
    if (status != UMI_STATUS_OK || bytes != KERNEL_FAT16_FIXTURE_BYTES ||
        strcmp(report->fixtureHash, report->diskAfterWriterHash) == 0) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "Disk changed incorrectly or was unchanged after the accepted writer.");
        return report->status;
    }
    report->stagedOrFinishedVerified = 1;
    return UMI_STATUS_OK;
}

UmiStatus UmiVmKernelDiskQualificationRun(
    const UmiVmKernelDiskRequest *request, UmiVmKernelDiskReport *report)
{
    if (!report) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    report->phase = UMI_VM_KERNEL_DISK_PHASE_VALIDATE;
    UmiStatus status = UmiVmKernelDiskRequestValidate(request);
    if (status != UMI_STATUS_OK) {
        KernelFailure(report, status, "Invalid Kernel disposable-disk qualification options.");
        return status;
    }
    if (UmiSetupFileCheck(request->workingDirectory, 1, NULL) != UMI_STATUS_OK ||
        UmiSetupNativeProgramCheck(request->qemuExecutable, NULL) != UMI_STATUS_OK ||
        UmiSetupNativeProgramCheck(request->verifier, NULL) != UMI_STATUS_OK ||
        UmiSetupFileCheck(request->firmware, 0, NULL) != UMI_STATUS_OK) {
        KernelFailure(report, UMI_STATUS_NOT_FOUND,
            "An explicit QEMU, verifier, working-directory or firmware path is missing.");
        return report->status;
    }
    report->phase = UMI_VM_KERNEL_DISK_PHASE_COPY;
    status = KernelScratchCreate(request, report);
    if (status != UMI_STATUS_OK) return status;
    report->phase = UMI_VM_KERNEL_DISK_PHASE_WRITER;
    status = KernelRunGuest(request, report, 0);
    if (status != UMI_STATUS_OK) return status;
    report->phase = UMI_VM_KERNEL_DISK_PHASE_VERIFY;
    status = KernelVerify(request, report);
    if (status != UMI_STATUS_OK) return status;
    report->phase = UMI_VM_KERNEL_DISK_PHASE_READER;
    status = KernelRunGuest(request, report, 1);
    if (status != UMI_STATUS_OK) return status;
    uint64_t bytes = 0U;
    status = UmiSetupFileDigest(report->disk, report->diskAfterReaderHash, &bytes, NULL);
    if (status != UMI_STATUS_OK || bytes != KERNEL_FAT16_FIXTURE_BYTES ||
        strcmp(report->diskAfterWriterHash, report->diskAfterReaderHash) != 0) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "The fresh read-only reboot changed the qualified disk image.");
        return report->status;
    }
    char originalHash[65];
    uint64_t originalBytes = 0U;
    status = UmiSetupFileDigest(request->fixture, originalHash, &originalBytes, NULL);
    if (status != UMI_STATUS_OK || originalBytes != KERNEL_FAT16_FIXTURE_BYTES ||
        strcmp(originalHash, report->fixtureHash) != 0) {
        KernelFailure(report, UMI_STATUS_INVALID_STATE,
            "The source fixture changed during qualification.");
        return report->status;
    }
    report->originalUnchanged = 1;
    report->phase = UMI_VM_KERNEL_DISK_PHASE_COMPLETE;
    report->status = UMI_STATUS_OK;
    (void)snprintf(report->details, sizeof report->details,
        "C23 Kernel disk qualification passed: verified complete disk, %u QEMU boots, "
        "fresh read-only boot unchanged; no repair performed.", report->bootsCompleted);
    return UMI_STATUS_OK;
}
