/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/os_image/image.h
 * Purpose: Native image assembly, frozen input plans and observed boot results.
 * Architecture: Host-side reusable work lives in Framework. Guest PID 1,
 * kernel choices and recovery policy remain in the independent OS repository.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OS_IMAGE_IMAGE_H
#define UMICOM_OS_IMAGE_IMAGE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OS_IMAGE_MAX_ENTRIES 128U
#define UMI_OS_IMAGE_MAX_INPUTS 256U
#define UMI_OS_IMAGE_PATH 4096U
#define UMI_OS_IMAGE_NAME 192U
#define UMI_OS_IMAGE_MAX_FILE (32U * 1024U * 1024U)
#define UMI_OS_IMAGE_MAX_ARCHIVE (64U * 1024U * 1024U)
#define UMI_OS_IMAGE_MAX_LOG (4U * 1024U * 1024U)
#define UMI_OS_IMAGE_MAX_BUILD_LOG (64U * 1024U * 1024U)

typedef enum UmiOsImageArch { UMI_OS_IMAGE_RISCV64 = 0, UMI_OS_IMAGE_X86_64 = 1 } UmiOsImageArch;
typedef enum UmiOsImageMode { UMI_OS_IMAGE_NORMAL = 0, UMI_OS_IMAGE_RECOVERY = 1 } UmiOsImageMode;
typedef struct UmiOsImageEntry {
    const char *name;
    uint32_t mode, major, minor;
    const unsigned char *data;
    size_t size;
} UmiOsImageEntry;
typedef struct UmiOsImageArchive UmiOsImageArchive;
typedef struct UmiOsImageReport {
    UmiStatus status;
    int outputCreated, completed, processLaunched, exitCode, timedOut, cancelled;
    size_t files;
    uint64_t bytes;
    char sourceId[65];
    char detail[384];
} UmiOsImageReport;

/** Buffer operations retain no caller memory. Newc is sorted and canonical;
 * gzip uses RFC 1951 stored blocks, so it needs no interpreter or compressor.
 * Open accepts canonical raw newc or this stored-gzip profile, not arbitrary
 * deflate streams. It does not extract files. Returned archive owns its bytes;
 * At returns borrowed views valid until Destroy. Build output uses free(). */
UmiStatus UmiOsImageArchiveBuild(const UmiOsImageEntry *entries, size_t count,
    unsigned char **outData, size_t *outSize);
UmiStatus UmiOsImageArchiveOpen(const void *data, size_t size, UmiOsImageArchive **outArchive);
size_t UmiOsImageArchiveCount(const UmiOsImageArchive *archive);
const UmiOsImageEntry *UmiOsImageArchiveAt(const UmiOsImageArchive *archive, size_t index);
void UmiOsImageArchiveDestroy(UmiOsImageArchive *archive);
UmiStatus UmiOsImageValidateName(const char *name);
UmiStatus UmiOsImageValidateElf(const void *data, size_t size, UmiOsImageArch arch);
UmiStatus UmiOsImageValidateKernel(const void *data, size_t size, UmiOsImageArch arch);

/** Assess a transcript only. Success means its ordered observations agree,
 * NOT that this call launched a guest, authenticated a publisher or proved
 * runtime isolation. Requires exactly one final canonical boot report and
 * the two expected successful service lifecycles (none in forced recovery). */
UmiStatus UmiOsImageAssessBoot(const void *data, size_t size, const char *sourceId,
    UmiOsImageMode mode, int processExit, UmiOsImageReport *report);

/** Linux/WSL host workflow. All roots must be absolute, non-overlapping and
 * free of spaces for Buildroot. prepare snapshots only the declared inventory
 * in image/native/profile.umi; it neither builds nor boots. gitProgram is an
 * explicitly trusted native executable. A clean pinned Buildroot is required.
 * Outputs are new directories, never merges. Partial output is retained. */
UmiStatus UmiOsImagePrepare(const char *osRoot, const char *frameworkRoot,
    const char *buildrootRoot, const char *destination, UmiOsImageArch arch,
    const char *gitProgram, UmiOsImageReport *report);
/** Configure, build and legal-info use the canonical native process runner,
 * not system()/shell text. The selected make executable and source recipes are
 * trusted build inputs, not sandboxed code. Non-root Linux account required.
 * Build rechecks the frozen inputs and exact configured .config. Failed builds
 * may be retried; a successful build marker is never overwritten. */
UmiStatus UmiOsImageConfigure(const char *prepared, const char *makeProgram,
    const UmiCancellationToken *cancel, UmiOsImageReport *report);
UmiStatus UmiOsImageBuild(const char *prepared, const char *makeProgram, unsigned jobs,
    const UmiCancellationToken *cancel, UmiOsImageReport *report);
UmiStatus UmiOsImageLegalInfo(const char *prepared, const char *makeProgram,
    const UmiCancellationToken *cancel, UmiOsImageReport *report);
/** Pack requires the successful native build marker and validated configs.
 * It reads three static guest programs and a fixed configuration whitelist;
 * no host device is created. The source manifest identifies captured bytes,
 * not publisher authenticity. Verify checks exact inventory and profile bytes,
 * never changes boot status or launches a process. */
UmiStatus UmiOsImagePack(const char *prepared, const char *destination, UmiOsImageReport *report);
UmiStatus UmiOsImageVerify(const char *bundle, UmiOsImageReport *report);
/** Automated direct-kernel QEMU run. Explicit native qemuProgram, fixed no-disk,
 * no-network arguments and bounded capture. A new result directory contains
 * console.log and result.umi. Success needs exit zero AND matching guest report.
 * UNAVAILABLE before execution is NOT RUN, not a passed/skipped boot. This is
 * neither BIOS/UEFI validation nor a hypervisor security boundary. */
UmiStatus UmiOsImageBoot(const char *bundle, const char *qemuProgram,
    UmiOsImageMode mode, unsigned timeoutSeconds, const char *resultDirectory,
    const UmiCancellationToken *cancel, UmiOsImageReport *report);
/** CLI adapter with an optional caller-owned cancellation token. The token
 * must remain alive until return; no process-global signal handler is installed
 * by the library. The standalone Linux host handles SIGINT/SIGTERM separately. */
int UmiOsImageMainWithCancellation(int argc, char **argv, const UmiCancellationToken *cancel);
int UmiOsImageMain(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
