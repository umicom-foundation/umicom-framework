/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/release_inspector/inspection.h
 * Purpose: Read-only inspection of an explicit Windows release or installation.
 * Architecture: Framework owns PE parsing and package-relative dependency
 * checks. No inspected executable or DLL is loaded; no PATH search is performed.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RELEASE_INSPECTOR_INSPECTION_H
#define UMICOM_RELEASE_INSPECTOR_INSPECTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/setup_centre/setup.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RELEASE_MAX_IMPORTS 512U
#define UMI_RELEASE_IMPORT_CAPACITY 256U
#define UMI_RELEASE_MAX_PE_BYTES (256U * 1024U * 1024U)
#define UMI_RELEASE_MAX_OBSERVATIONS 262144U
#define UMI_RELEASE_MACHINE_AMD64 0x8664U
/** This is a bounded PE metadata reader, not Windows' loader. The parser checks
 * the structures it follows, section mapping and normal/delay DLL names. It
 * does not prove instruction validity, exported functions, signatures or SxS
 * binding. Allocate this sizeable result on the heap in GUI/worker callers. */
typedef struct UmiReleasePeInfo {
    uint16_t machine;
    uint16_t optionalMagic;
    uint16_t subsystem;
    uint16_t characteristics;
    uint16_t sectionCount;
    uint32_t entryRva;
    uint32_t imageBytes;
    size_t importCount;
    char imports[UMI_RELEASE_MAX_IMPORTS][UMI_RELEASE_IMPORT_CAPACITY];
    unsigned char delayed[UMI_RELEASE_MAX_IMPORTS];
} UmiReleasePeInfo;
/** Input bytes are borrowed only for this call. Output is zeroed on failure.
 * RVA-based delay descriptors are supported; old VA-based descriptors return
 * UNAVAILABLE rather than being silently ignored. No operating-system calls. */
UmiStatus UmiReleasePeInspect(const void *bytes, size_t length, UmiReleasePeInfo *outInfo);

typedef enum UmiReleaseObservationKind {
    UMI_RELEASE_FILE_CHECKED = 0,
    UMI_RELEASE_PRIVATE_IMPORT = 1,
    UMI_RELEASE_SYSTEM_IMPORT = 2,
    UMI_RELEASE_ISSUE = 3,
    UMI_RELEASE_RESOURCE_CHECKED = 4
} UmiReleaseObservationKind;
typedef struct UmiReleaseObservation {
    UmiReleaseObservationKind kind;
    UmiStatus status;
    const char *path;
    const char *dependency;
    const char *detail;
    int delayed;
} UmiReleaseObservation;
/** Called synchronously on the inspecting thread. All strings are borrowed
 * until the callback returns. Return nonzero to cancel; never touch widgets or
 * destroy the inspected bundle from the callback. */
typedef int (*UmiReleaseObserver)(const UmiReleaseObservation *item, void *context);
typedef struct UmiReleaseInspection {
    UmiStatus status;
    size_t filesChecked;
    size_t imagesChecked;
    size_t privateImports;
    size_t systemImports;
    size_t delayedImports;
    size_t resourcesChecked;
    size_t issues;
    size_t observations;
    uint64_t bytesChecked;
    int complete;
    int runtimeTested; /* Always zero: this API never starts or loads a program. */
    char firstIssue[320];
} UmiReleaseInspection;
/* Extended provider profile: paths below share/umicom/qemu/ resolve private
 * imports ONLY against share/umicom/qemu/bin/. The general bin rule below is
 * retained for existing applications; the isolated provider must not satisfy
 * an import accidentally from the unrelated Umicom runtime directory. */
/** Reads selected/shared inventory. Only bin/<name>.exe application entries and
 * bin/<name>.dll private dependency resolution are qualified by this profile.
 * DLLs in resource/module subdirectories are also inspected, but their normal
 * imports must resolve to bin/ or the explicit Windows-system classification.
 * Windows-system names/API sets are DEFERRED requirements, not verified files.
 * Additional dynamically loaded files are governed by the original inventory;
 * arbitrary LoadLibrary names, export symbols and host API support are not proved.
 * All selected files are hashed. At most one bounded PE image is held in RAM.
 * OK means the static check completed without issues; it never means launch-ready.
 * Changes to catalogue bytes during a scan invalidate the result. An observer
 * may receive a FILE_CHECKED heartbeat with empty path during hashing; that is
 * not a completed-file observation and is excluded from the counters. */
UmiStatus UmiReleaseInspectBundle(const UmiSetupBundle *bundle, uint64_t selected,
    UmiReleaseObserver observer, void *context, UmiReleaseInspection *outInspection);
/** Same inspection against an installation receipt. Unlisted personal files
 * are neither inspected nor removed. Receipt integrity is not authenticity. */
UmiStatus UmiReleaseInspectInstallation(const char *root, UmiReleaseObserver observer,
    void *context, UmiReleaseInspection *outInspection);
/** One-line summary, always stating that no application was launched. */
UmiStatus UmiReleaseInspectionSummary(const UmiReleaseInspection *inspection,
    char *outText, size_t capacity);
/** Console interface: release|installation|pe --root/--file ABSOLUTE;
 * --output NEW_ABSOLUTE writes a bounded JSON report exclusively. */
int UmiReleaseInspectorMain(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
