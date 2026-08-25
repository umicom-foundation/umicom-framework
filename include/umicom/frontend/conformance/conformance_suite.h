/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_suite.h
 *
 * PURPOSE:
 *   bounded ordered collection of conformance rules for one application or workstation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcConformanceSuite { char id[UMI_FC_ID_CAPACITY]; UmiFcConformanceRule rules[UMI_FC_MAX_ITEMS]; size_t count; } UmiFcConformanceSuite;
void umi_fc_conformance_suite_init(UmiFcConformanceSuite *suite,const char *id);
UmiStatus umi_fc_conformance_suite_add(UmiFcConformanceSuite *suite,const UmiFcConformanceRule *rule);
size_t umi_fc_conformance_suite_blocker_count(const UmiFcConformanceSuite *suite);

#ifdef __cplusplus
}
#endif
#endif
