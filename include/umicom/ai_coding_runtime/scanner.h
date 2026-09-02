/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/scanner.h
 *
 * PURPOSE:
 *   Enumerate workspace files recursively through a platform implementation,
 *   yielding normalized repository-relative paths and basic file metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_SCANNER_H
#define UMICOM_AI_CODING_RUNTIME_SCANNER_H

#include "umicom/ai_coding_runtime/ignore.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding scan entry data shared with callers of this public contract.
 */
typedef struct UmiAiCodingScanEntry {
    char relative_path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    uint64_t byte_size;
    int directory;
} UmiAiCodingScanEntry;

typedef UmiStatus (*UmiAiCodingScanVisitor)(
    void *user_data,
    const UmiAiCodingScanEntry *entry,
    int *out_descend);

/**
 * Provide the ai coding scan workspace operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_scan_workspace(
    const char *root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *out_file_count);

#ifdef __cplusplus
}
#endif
#endif
