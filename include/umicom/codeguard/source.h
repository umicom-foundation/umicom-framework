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
UmiStatus umi_codeguard_source_measure(const char *path, UmiCodeGuardSourceInfo *out_info);
#ifdef __cplusplus
}
#endif
#endif
