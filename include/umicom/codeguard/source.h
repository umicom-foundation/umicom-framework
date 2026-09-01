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
typedef struct UmiCodeGuardSourceInfo {
    char path[1024];
    size_t bytes;
    size_t lines;
    size_t include_count;
    size_t function_hint_count;
} UmiCodeGuardSourceInfo;
int umi_codeguard_source_supported(const char *path, int include_cpp, int include_headers);
UmiStatus umi_codeguard_source_measure(const char *path, UmiCodeGuardSourceInfo *out_info);
#ifdef __cplusplus
}
#endif
#endif
