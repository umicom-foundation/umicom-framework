/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/artifact_inventory.h
 * PURPOSE: Inspect declared installed SDK artifacts without executing package code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_ARTIFACT_INVENTORY_H
#define UMICOM_SDK_ARTIFACT_INVENTORY_H
#include "umicom/base/status.h"
#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SDK_ARTIFACT_BYTE_LIMIT (1024U * 1024U)
#define UMI_SDK_ARTIFACT_COUNT_LIMIT 4096U
#define UMI_SDK_ARTIFACT_NAME_CAPACITY 128U
#define UMI_SDK_ARTIFACT_PATH_CAPACITY 512U
#define UMI_SDK_ARTIFACT_INVENTORY_PATH "share/umicom/framework/sdk-artifacts.txt"

typedef enum UmiSdkArtifactKind {
    UMI_SDK_ARTIFACT_ARCHIVE, UMI_SDK_ARTIFACT_SHARED,
    UMI_SDK_ARTIFACT_IMPORT, UMI_SDK_ARTIFACT_TOOL, UMI_SDK_ARTIFACT_INTERFACE
} UmiSdkArtifactKind;
typedef struct UmiSdkArtifactEntry {
    char target[UMI_SDK_ARTIFACT_NAME_CAPACITY];
    char relative_path[UMI_SDK_ARTIFACT_PATH_CAPACITY];
    UmiSdkArtifactKind kind;
    bool present;
} UmiSdkArtifactEntry;
typedef struct UmiSdkArtifactSummary {
    char configuration[64];
    size_t entries;
    size_t files;
    size_t present;
    size_t missing;
    bool inspected;
} UmiSdkArtifactSummary;
typedef struct UmiSdkArtifactInventory UmiSdkArtifactInventory;

/** Parse the bounded data-only receipt into an owned immutable inventory.
 * Output is NULL on failure. Input may be released after success. Records need
 * a final newline; duplicate target/kind pairs, unsafe relative paths, unknown
 * kinds and malformed text are rejected. Parsing does not inspect files. */
UmiStatus UmiSdkArtifactInventoryParse(const void *bytes, size_t length,
    UmiSdkArtifactInventory **out_inventory);
/** Read the receipt from an explicitly selected absolute prefix and observe all
 * declared files. Missing files are observations, not parse errors. Select a
 * trusted installation: presence checks can follow filesystem aliases. No CMake,
 * compiler or application executes. Evidence does not prove artifact contents,
 * ABI, source revision, unlisted exports or atomic filesystem consistency.
 * Run this synchronous filesystem observation on a worker when used by a UI. */
UmiStatus UmiSdkArtifactInventoryInspect(const char *prefix,
    UmiSdkArtifactInventory **out_inventory);
/** Copy summary evidence. An uninspected parsed receipt has zero observed counts. */
UmiStatus UmiSdkArtifactInventoryReadSummary(const UmiSdkArtifactInventory *inventory,
    UmiSdkArtifactSummary *out_summary);
/** Borrow a record until inventory destruction; return NULL for an invalid index. */
const UmiSdkArtifactEntry *UmiSdkArtifactInventoryEntryAt(const UmiSdkArtifactInventory *inventory,
    size_t index);
/** Release owned evidence; NULL is harmless. */
void UmiSdkArtifactInventoryDestroy(UmiSdkArtifactInventory *inventory);
#ifdef __cplusplus
}
#endif
#endif
