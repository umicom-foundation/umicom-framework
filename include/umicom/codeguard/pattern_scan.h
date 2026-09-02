/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/pattern_scan.h
 *
 * PURPOSE:
 *   Apply the registered pattern rules to one source line.
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
#ifndef UMICOM_CODEGUARD_PATTERN_SCAN_H
#define UMICOM_CODEGUARD_PATTERN_SCAN_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/rule_registry.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard pattern scan line operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_pattern_scan_line(const UmiCodeGuardRuleRegistry *registry,
                                          const char *path,
                                          size_t line_number,
                                          const char *raw_line,
                                          const char *code_line,
                                          UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
