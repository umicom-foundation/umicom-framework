/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/lifetime.h
 *
 * PURPOSE:
 *   Track simple pointer lifetime events to detect obvious double-free and
 *   use-after-free patterns.
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
#ifndef UMICOM_CODEGUARD_LIFETIME_H
#define UMICOM_CODEGUARD_LIFETIME_H
#include <stddef.h>
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCodeGuardLifetimeTracker {
    char freed_names[64][96];
    size_t freed_lines[64];
    size_t count;
} UmiCodeGuardLifetimeTracker;
void umi_codeguard_lifetime_init(UmiCodeGuardLifetimeTracker *tracker);
void umi_codeguard_lifetime_scan(UmiCodeGuardLifetimeTracker *tracker,
                                 const char *path, size_t line_number,
                                 const char *code_line, UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
