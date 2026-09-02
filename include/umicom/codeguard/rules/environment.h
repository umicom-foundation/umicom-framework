/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rules/environment.h
 *
 * PURPOSE:
 *   Declare the environment CodeGuard rule pack.
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
#ifndef UMICOM_CODEGUARD_RULES_ENVIRONMENT_H
#define UMICOM_CODEGUARD_RULES_ENVIRONMENT_H
#include <stddef.h>
#include "umicom/codeguard/rule.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard rules environment operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_environment(size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
