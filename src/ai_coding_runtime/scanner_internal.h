/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/scanner_internal.h
 *
 * PURPOSE:
 *   Internal operating-system boundary for recursive repository scanning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_SCANNER_INTERNAL_H
#define UMICOM_AI_CODING_RUNTIME_SCANNER_INTERNAL_H
#include "umicom/ai_coding_runtime/scanner.h"
UmiStatus umi_ai_coding_platform_scan_workspace(
    const char *root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *out_file_count);
#endif
