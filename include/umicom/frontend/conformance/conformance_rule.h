/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_rule.h
 *
 * PURPOSE:
 *   weighted conformance rules with severity and required capability evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_RULE_H
#define UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc conformance rule data shared with callers of this public contract.
 */
typedef struct UmiFcConformanceRule { char id[UMI_FC_ID_CAPACITY]; UmiFcSeverity severity; uint64_t required_capabilities; double weight; } UmiFcConformanceRule;
/**
 * Provide the fc conformance rule make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_conformance_rule_make(const char *id,UmiFcSeverity severity,uint64_t required_capabilities,double weight,UmiFcConformanceRule *out_rule);
/**
 * Provide the fc conformance rule passes operation used by this module and its client
 * applications.
 */
bool umi_fc_conformance_rule_passes(const UmiFcConformanceRule *rule,uint64_t actual);

#ifdef __cplusplus
}
#endif
#endif
