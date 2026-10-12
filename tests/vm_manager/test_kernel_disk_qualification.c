/*-----------------------------------------------------------------------------
 *
 * File: tests/vm_manager/test_kernel_disk_qualification.c
 *
 * PURPOSE: Check disk qualification ownership and failure handling with simulated services.
 *
 * LICENCE: MIT
 *
 * Umicom Framework - C23 Kernel disposable FAT16 qualification service tests.
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 *
 * These are deterministic transport/file-service substitutes. They exercise
 * the real qualification coordinator's ownership and phase logic without
 * starting QEMU or writing a disk. Actual C23 verifier + guest acceptance
 * must be separately run on the Windows/RV64 development host.
 *---------------------------------------------------------------------------*/
#include "umicom/vm_manager/kernel_disk_qualification.h"
#include "umicom/setup_centre/files.h"
#include "umicom/platform/process.h"
#include "boot_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

#define KNOWN_HASH "bd866d6ae337527f3a8b4609f31969d60185e025f8525726f573d905a2f50938"
#define WRITTEN_HASH "b38d4c1def74b0241a91a350362038a85934a9a5d522f86bd7c5347b44c3e1b5"
#define DIFFERENT_HASH "c38d4c1def74b0241a91a350362038a85934a9a5d522f86bd7c5347b44c3e1b5"

struct UmiVmSession { int readonly; };
static unsigned checks;
static int existsDirectory,wrongFixture,copyFailure,writerFailure,readerFailure;
static int verifierFailure,readerChanged,sourceChanged,stopFailure,bootFailure,missingExe;
static int createdCount,bootCount,stopCount,verifyCount,copyCount;
static int diskChanged,readOnlyBoots;
static char diskPath[UMI_VM_PATH];

static UmiVmKernelDiskRequest Request(UmiVmKernelDiskCase exercise)
{
    UmiVmKernelDiskRequest value = {0};
    value.qemuExecutable = "/host/qemu";
    value.firmware = "/host/firmware.elf";
    value.workingDirectory = "/host/work";
    value.fixture = "/host/fixture.raw";
    value.outputDirectory = "/host/NEW-evidence";
    value.verifier = "/host/verifier";
    value.exercise = exercise;
    value.blockSlot = 7U;
    value.commandTimeoutMilliseconds = 1000U;
    return value;
}

static void Reset(void)
{
    existsDirectory = wrongFixture = copyFailure = writerFailure = readerFailure = 0;
    verifierFailure = readerChanged = sourceChanged = stopFailure = bootFailure = missingExe = 0;
    createdCount = bootCount = stopCount = verifyCount = copyCount = 0;
    diskChanged = readOnlyBoots = 0;
    diskPath[0] = 0;
}

int VmPath(const char *text, int optional)
{
    (void)optional;
    return text && text[0] == '/';
}

UmiStatus UmiSetupFileCheck(const char *p, int directory, UmiSetupReport *report)
{
    (void)report;
    if (directory) {
        if (strcmp(p, "/host/NEW-evidence") == 0)
            return existsDirectory ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
        return UMI_STATUS_OK;
    }
    if (diskPath[0] && strcmp(p, diskPath) == 0)
        return diskChanged || copyCount ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

UmiStatus UmiSetupFileDigest(const char *p, char digest[65], uint64_t *bytes,
    UmiSetupReport *report)
{
    (void)report;
    if (strcmp(p, "/host/fixture.raw") == 0)
        strcpy(digest, wrongFixture || (sourceChanged && readOnlyBoots) ?
            DIFFERENT_HASH : KNOWN_HASH);
    else if (diskPath[0] && strcmp(p, diskPath) == 0)
        strcpy(digest, readerChanged && readOnlyBoots ? DIFFERENT_HASH :
            diskChanged ? WRITTEN_HASH : KNOWN_HASH);
    else return UMI_STATUS_NOT_FOUND;
    *bytes = UINT64_C(8388608);
    return UMI_STATUS_OK;
}

UmiStatus UmiSetupDirectoryCreate(const char *path, UmiSetupReport *report)
{
    (void)path; (void)report;
    ++createdCount;
    return UMI_STATUS_OK;
}
UmiStatus UmiSetupPathJoin(const char *dir, const char *leaf,
    char out[UMI_SETUP_PATH_CAPACITY])
{
    int n = snprintf(out, UMI_SETUP_PATH_CAPACITY, "%s/%s", dir, leaf);
    if (n < 0 || n >= (int)UMI_SETUP_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(diskPath, out);
    return UMI_STATUS_OK;
}
UmiStatus UmiSetupFileCopyChecked(const char *source, const char *target,
    const char *hash, uint64_t count, UmiSetupReport *report)
{
    (void)source; (void)target; (void)hash; (void)count; (void)report;
    ++copyCount;
    return copyFailure ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
UmiStatus UmiSetupFileSingleLink(const char *path, UmiSetupReport *report)
{
    (void)path; (void)report;
    return UMI_STATUS_OK;
}
UmiStatus UmiSetupNativeProgramCheck(const char *path, UmiSetupReport *report)
{
    (void)path; (void)report;
    return missingExe ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}

UmiStatus UmiVmBootRequestInit(UmiVmBootRequest *r, UmiVmBootTarget target)
{
    memset(r, 0, sizeof *r);
    r->target = target;
    r->memoryMiB = 128U;
    r->processors = 1U;
    return UMI_STATUS_OK;
}
UmiStatus UmiVmBootPlanCreate(const UmiVmBootRequest *r, UmiVmBootPlan **out)
{
    *out = calloc(1, sizeof **out);
    if (!*out) return UMI_STATUS_IO_ERROR;
    (*out)->request = *r;
    (*out)->argumentCount = 6U;
    strcpy((*out)->arguments[0], "-machine");
    strcpy((*out)->arguments[1], "virt,aclint=off");
    strcpy((*out)->arguments[2], "-drive");
    strcpy((*out)->arguments[3],
        "if=none,id=umicom_boot_disk,format=raw,snapshot=on,file=/host/NEW-evidence/umicom-kernel-owned-fat16.raw");
    strcpy((*out)->arguments[4], "-device");
    strcpy((*out)->arguments[5], "virtio-blk-device,drive=umicom_boot_disk");
    return UMI_STATUS_OK;
}
void UmiVmBootPlanDestroy(UmiVmBootPlan *plan) { free(plan); }
UmiStatus UmiVmBootReview(const UmiVmBootPlan *plan, UmiVmReport *report)
{
    (void)plan;
    strcpy(report->fingerprint, KNOWN_HASH);
    return UMI_STATUS_OK;
}
UmiStatus UmiVmBootStart(const UmiVmBootPlan *plan, const char *hash,
    UmiVmSession **out, UmiVmReport *report)
{
    (void)hash; (void)report;
    if (bootFailure) return UMI_STATUS_IO_ERROR;
    *out = calloc(1, sizeof **out);
    if (!*out) return UMI_STATUS_IO_ERROR;
    (*out)->readonly = strstr(plan->arguments[3], "readonly=on") != NULL;
    ++bootCount;
    if ((*out)->readonly) ++readOnlyBoots;
    return UMI_STATUS_OK;
}
UmiStatus UmiVmControl(UmiVmSession *session, UmiVmCommand command,
    const void *input, size_t n, void *output, size_t capacity, size_t *outLength,
    UmiVmReport *report)
{
    (void)session; (void)command; (void)input; (void)n;
    (void)output; (void)capacity; (void)outLength; (void)report;
    return UMI_STATUS_OK;
}
UmiVmConsoleJourneyStatus UmiVmConsoleJourneyRunNative(UmiVmSession *session,
    const UmiVmConsoleJourneyStep *steps, size_t count, uint64_t timeout,
    UmiVmConsoleJourneyReport *report)
{
    (void)timeout;
    report->status = UMI_VM_CONSOLE_JOURNEY_OK;
    if (session->readonly) {
        if (readerFailure) {
            report->failedStep = 1U;
            strcpy(report->detail, "mock-readonly-refusal");
            return UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE;
        }
        if (count != (strcmp(steps[1].expectedText, "disk.media=unclean-volume") == 0 ? 2U : 3U)) {
            /* In the model, whether Stage/Finish controls the expected
             * reader command count is tested by looking at the actual data. */
            return UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE;
        }
    } else {
        if (writerFailure) {
            report->failedStep = 2U;
            strcpy(report->detail, "mock-writer-refusal");
            return UMI_VM_CONSOLE_JOURNEY_GUEST_FAILURE;
        }
        if (count < 4U || strcmp(steps[1].expectedText, "fat.commit.open=ok") != 0)
            return UMI_VM_CONSOLE_JOURNEY_PROTOCOL_FAILURE;
        diskChanged = 1;
    }
    report->completedSteps = count;
    return UMI_VM_CONSOLE_JOURNEY_OK;
}
UmiStatus UmiVmForceStop(UmiVmSession *session, UmiVmReport *report)
{
    (void)session; (void)report;
    ++stopCount;
    return stopFailure ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
void UmiVmSessionDestroy(UmiVmSession *session) { free(session); }
UmiStatus umi_process_execute(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++verifyCount;
    if (strcmp(request->program, "/host/verifier") != 0 ||
        request->argument_count != 3U ||
        (strcmp(request->arguments[2], "stage") != 0 &&
         strcmp(request->arguments[2], "finish") != 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    result->launched = 1;
    result->exit_code = verifierFailure ? 1 : 0;
    strcpy(result->output, verifierFailure ? "{\"classification\":\"bytes-differ\"}" :
        "{\"classification\":\"verified\",\"full_image_compared\":true,\"repair_performed\":false}");
    return UMI_STATUS_OK;
}

static int TestGood(UmiVmKernelDiskCase exercise)
{
    Reset();
    UmiVmKernelDiskRequest request = Request(exercise);
    UmiVmKernelDiskReport report;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) == UMI_STATUS_OK);
    EXPECT(report.phase == UMI_VM_KERNEL_DISK_PHASE_COMPLETE);
    EXPECT(report.status == UMI_STATUS_OK);
    EXPECT(report.bootsCompleted == 2U);
    EXPECT(report.readerObserved && report.stagedOrFinishedVerified && report.originalUnchanged);
    EXPECT(bootCount == 2 && stopCount == 2 && copyCount == 1 && verifyCount == 1);
    EXPECT(strcmp(report.diskAfterWriterHash, WRITTEN_HASH) == 0);
    EXPECT(strcmp(report.diskAfterReaderHash, report.diskAfterWriterHash) == 0);
    return 0;
}

static int TestValidation(void)
{
    Reset();
    UmiVmKernelDiskRequest request = Request(UMI_VM_KERNEL_DISK_STAGED);
    EXPECT(UmiVmKernelDiskQualificationRun(&request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiVmKernelDiskReport report;
    request.blockSlot = 8U;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) == UMI_STATUS_INVALID_ARGUMENT);
    EXPECT(createdCount == 0 && bootCount == 0);
    request = Request(UMI_VM_KERNEL_DISK_STAGED);
    request.qemuExecutable = "relative/qemu";
    EXPECT(UmiVmKernelDiskRequestValidate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    request = Request(UMI_VM_KERNEL_DISK_STAGED);
    request.commandTimeoutMilliseconds = 99U;
    EXPECT(UmiVmKernelDiskRequestValidate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    request = Request(UMI_VM_KERNEL_DISK_STAGED);
    request.exercise = (UmiVmKernelDiskCase)3;
    EXPECT(UmiVmKernelDiskRequestValidate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    request = Request(UMI_VM_KERNEL_DISK_STAGED);
    request.outputDirectory = request.workingDirectory;
    EXPECT(UmiVmKernelDiskRequestValidate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

static int TestFailures(void)
{
    UmiVmKernelDiskRequest request = Request(UMI_VM_KERNEL_DISK_STAGED);
    UmiVmKernelDiskReport report;
    Reset(); existsDirectory = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(copyCount == 0 && bootCount == 0);
    Reset(); wrongFixture = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(copyCount == 0 && bootCount == 0);
    Reset(); missingExe = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(copyCount == 0 && bootCount == 0);
    Reset(); copyFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(copyCount == 1 && bootCount == 0);
    Reset(); bootFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 0 && bootCount == 0);
    Reset(); writerFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 0 && bootCount == 1 && stopCount == 1);
    Reset(); verifierFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 1 && bootCount == 1 && stopCount == 1);
    Reset(); readerFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 1 && bootCount == 2 && stopCount == 2);
    Reset(); readerChanged = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 1 && bootCount == 2);
    Reset(); sourceChanged = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 1 && bootCount == 2);
    Reset(); stopFailure = 1;
    EXPECT(UmiVmKernelDiskQualificationRun(&request, &report) != UMI_STATUS_OK);
    EXPECT(verifyCount == 0 && bootCount == 1 && stopCount == 1);
    return 0;
}

int main(void)
{
    if (TestGood(UMI_VM_KERNEL_DISK_STAGED) != 0) return 1;
    if (TestGood(UMI_VM_KERNEL_DISK_FINISHED) != 0) return 1;
    if (TestValidation() != 0) return 1;
    if (TestFailures() != 0) return 1;
    printf("UMICOM_KERNEL_DISK_QUALIFICATION_READY checks=%u\n", checks);
    return 0;
}
