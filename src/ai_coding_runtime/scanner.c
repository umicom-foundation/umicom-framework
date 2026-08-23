/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/scanner.c
 *
 * PURPOSE:
 *   Validate repository-scan inputs and delegate only the directory walking to
 *   the platform adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "scanner_internal.h"

UmiStatus umi_ai_coding_scan_workspace(
    const char *root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *out_file_count)
{
    if (root == NULL || root[0] == '\0' ||
        ignore_policy == NULL ||
        visitor == NULL ||
        out_file_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_file_count = 0U;

    return umi_ai_coding_platform_scan_workspace(
        root,
        ignore_policy,
        visitor,
        user_data,
        out_file_count);
}
