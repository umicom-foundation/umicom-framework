/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_suite.h
 *
 * PURPOSE:
 *   bounded ordered collection of conformance rules for one application or workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_SUITE_H
#define UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_SUITE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/conformance_rule.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc conformance suite data shared with callers of this public contract.
 */
typedef struct UmiFcConformanceSuite { char id[UMI_FC_ID_CAPACITY]; UmiFcConformanceRule rules[UMI_FC_MAX_ITEMS]; size_t count; } UmiFcConformanceSuite;
/**
 * Initialise fc conformance suite from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_conformance_suite_init(UmiFcConformanceSuite *suite,const char *id);
/**
 * Add fc conformance suite only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fc_conformance_suite_add(UmiFcConformanceSuite *suite,const UmiFcConformanceRule *rule);
/**
 * Return the number of records represented by fc conformance suite blocker without
 * changing their state.
 */
size_t umi_fc_conformance_suite_blocker_count(const UmiFcConformanceSuite *suite);

#ifdef __cplusplus
}
#endif
#endif
