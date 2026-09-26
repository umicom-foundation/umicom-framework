/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: include/umicom/native_launcher/stage.h
 * Purpose: Native, reviewable Linux launcher delivery with no interpreter.
 * Architecture: Framework owns validation, encoding and filesystem operations.
 * Thin products supply paths. Staging never executes an input, changes the
 * desktop, installs autostart, or edits an existing destination.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_NATIVE_LAUNCHER_STAGE_H
#define UMICOM_NATIVE_LAUNCHER_STAGE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NATIVE_STAGE_PATH_CAPACITY 4096U
#define UMI_NATIVE_STAGE_TEXT_CAPACITY 65536U
#define UMI_NATIVE_STAGE_ICON_LIMIT 1048576U
#define UMI_NATIVE_STAGE_BINARY_LIMIT UINT64_C(536870912)
#define UMI_NATIVE_STAGE_SCHEMA "umicom.desktop-session-stage"
#define UMI_NATIVE_STAGE_SCOPE "References an existing Linux Desk installation; no compositor, login manager, autostart or boot image installed."
#define UMI_NATIVE_STAGE_ICON_NAME "umicom-icon.png"
#define UMI_NATIVE_STAGE_ENTRY_NAME "umicom-desk-session.desktop"
#define UMI_NATIVE_STAGE_MANIFEST_NAME "manifest.json"
/** All strings are borrowed for the duration of PlanCreate. NULL iconPath uses
 * the compiled-in Umicom PNG; an override is a bounded PNG, not a brand check.
 * Paths must be canonical absolute UTF-8 Linux paths without %, control bytes,
 * dot segments, repeated slashes or symbolic-link components. The launcher
 * token also rejects an equals sign and a trailing space (TryExec ambiguity). */
typedef struct UmiNativeStageConfig {
    const char *deskPath;
    const char *launcherPath;
    const char *outputPath;
    const char *iconPath;
} UmiNativeStageConfig;
typedef struct UmiNativeStageBinary {
    char path[UMI_NATIVE_STAGE_PATH_CAPACITY];
    char sha256[65];
    uint64_t bytes;
    uint16_t machine;
} UmiNativeStageBinary;
/** The existing Python manifest schema is retained. New native verification is
 * deliberately stricter: exact keys, exact directory inventory, structural PNG
 * validation and regenerated desktop-entry comparison in addition to hashes. */
typedef struct UmiNativeStageManifest {
    char root[UMI_NATIVE_STAGE_PATH_CAPACITY];
    UmiNativeStageBinary desk;
    UmiNativeStageBinary launcher;
    char iconHash[65];
    char entryHash[65];
} UmiNativeStageManifest;
typedef struct UmiNativeStageReport {
    UmiStatus status;
    int systemError;
    int outputCreated;
    int completed;
    char phase[40];
    char detail[256];
} UmiNativeStageReport;
typedef struct UmiNativeStagePlan UmiNativeStagePlan;
/** PlanCreate reads and fingerprints inputs, retaining only icon bytes and
 * generated text, not either executable. It makes no filesystem changes.
 * Plan objects are single-threaded; Destroy accepts NULL. */
UmiStatus UmiNativeStagePlanCreate(const UmiNativeStageConfig *config,
    UmiNativeStagePlan **outPlan, UmiNativeStageReport *report);
void UmiNativeStagePlanDestroy(UmiNativeStagePlan *plan);
const UmiNativeStageManifest *UmiNativeStagePlanManifest(const UmiNativeStagePlan *plan);
const char *UmiNativeStagePlanEntry(const UmiNativeStagePlan *plan);
const char *UmiNativeStagePlanFingerprint(const UmiNativeStagePlan *plan);
/** A commit attempt consumes the plan, even on failure. expectedFingerprint
 * may be NULL; otherwise it must match a separately reviewed plan. Executables are
 * re-read before directory creation and again before the completion manifest.
 * The icon is an owned snapshot; its original file is not re-read by Commit.
 * Partial outputs are retained and never removed or silently retried.
 * A successful return is integrity evidence for this observation, not a lock
 * against future input changes or an assertion that Desk can run. */
UmiStatus UmiNativeStageCommit(UmiNativeStagePlan *plan,
    const char *expectedFingerprint, UmiNativeStageReport *report);
/** Read only. Refuses moved stages, unexpected files and changed executables.
 * Checksums are not signatures: the manifest and its author must be trusted. */
UmiStatus UmiNativeStageVerify(const char *outputPath, UmiNativeStageReport *report);
/** Portable, bounded codecs: no filesystem or process activity. */
UmiStatus UmiNativeStageValidatePath(const char *path, int executableToken);
UmiStatus UmiNativeStageRenderEntry(const UmiNativeStageManifest *manifest,
    char *outText, size_t capacity, size_t *outLength);
UmiStatus UmiNativeStageManifestWrite(const UmiNativeStageManifest *manifest,
    char *outText, size_t capacity, size_t *outLength);
UmiStatus UmiNativeStageManifestRead(const char *text, size_t length,
    UmiNativeStageManifest *outManifest);
UmiStatus UmiNativeStagePngCheck(const unsigned char *data, size_t length);
UmiStatus UmiNativeStageElfCheck(const unsigned char *header, size_t headerLength,
    uint64_t fileLength, uint16_t *outMachine);
const char *UmiNativeStageStatusText(UmiStatus status);
/** Read-only streaming digest of a regular file. Linux opens relative to checked
 * directory descriptors; Windows uses a non-reparse-point file handle. No
 * executable admission or trust assertion is implied by this digest API. */
UmiStatus UmiNativeStageDigestFile(const char *path, char outHex[65], uint64_t *outBytes,
    UmiNativeStageReport *report);
/** CLI shares the same implementation as the API; no shell commands are used. */
int UmiNativeStageMain(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
