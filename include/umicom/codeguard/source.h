/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/source.h
 *
 * PURPOSE:
 *   Recognise source files and measure basic file, line and include metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SOURCE_H
#define UMICOM_CODEGUARD_SOURCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/platform/cancellation.h"
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard source info data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardSourceInfo {
    char path[1024];
    size_t bytes;
    size_t lines;
    size_t include_count;
    size_t function_hint_count;
} UmiCodeGuardSourceInfo;
/**
 * Provide the codeguard source supported operation used by this module and its client
 * applications.
 */
int umi_codeguard_source_supported(const char *path, int include_cpp, int include_headers);
/**
 * Provide the codeguard source measure operation used by this module and its client
 * applications.
 */
/* Success replaces out_info; any error leaves it unchanged. Paths longer
 * than 1023 bytes are refused instead of truncated. Read and close failures
 * are reported. Metrics retain the existing fixed-buffer textual heuristic;
 * this API is not a syntax parser or an atomic snapshot of a changing file. */
UmiStatus umi_codeguard_source_measure(const char *path, UmiCodeGuardSourceInfo *out_info);

/* Bounded native alternative: absolute UTF-8 or a relative path resolved
 * once per call. Only regular final leaves are admitted; final symbolic links
 * and reparse points are refused. maximum_bytes==0 admits empty files only.
 * Text containing NUL is INVALID_ARGUMENT; missing input remains IO_ERROR.
 * Output is unchanged on any error. Cancellation is cooperative. The legacy
 * stdio API above remains available for compatibility. */
UmiStatus umi_codeguard_source_measure_bounded(const char *path,
    uint64_t maximum_bytes, UmiCodeGuardSourceInfo *out_info,
    const UmiCancellationToken *cancellation);
#ifdef __cplusplus
}
#endif
#endif
