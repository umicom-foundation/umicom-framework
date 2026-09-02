/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/unsafe_api.h
 *
 * PURPOSE:
 *   Publish the public unsafe api contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_UNSAFE_API_H
#define UMICOM_CODEGUARD_UNSAFE_API_H
#include "umicom/codeguard/evidence.h"
/**
 * Represent the code guard unsafe api rule data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardUnsafeApiRule {
    const char *token;
    const char *rule_id;
    const char *replacement;
    UmiCodeGuardEvidenceState state;
} UmiCodeGuardUnsafeApiRule;
/**
 * Return the number of records represented by codeguard unsafe api rule without changing
 * their state.
 */
size_t umi_codeguard_unsafe_api_rule_count(void);
/**
 * Find codeguard unsafe api rule while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCodeGuardUnsafeApiRule *umi_codeguard_unsafe_api_rule_at(size_t index);
/**
 * Provide the codeguard unsafe api scan line operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_unsafe_api_scan_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);
#endif
