/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rules/private_header.h
 *
 * PURPOSE:
 *   Declare the private header CodeGuard rule pack.
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
#ifndef UMICOM_CODEGUARD_RULES_PRIVATE_HEADER_H
#define UMICOM_CODEGUARD_RULES_PRIVATE_HEADER_H
#include <stddef.h>
#include "umicom/codeguard/rule.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard rules private header operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_private_header(size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
