/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/acceptance_rule.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_RULE_H
#define UMICOM_APPLICATION_PRODUCTION_ACCEPTANCE_RULE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/types.h"

typedef struct UmiApplicationProductionAcceptanceRule {
    int require_manifest;
    int require_layout_projection;
    int require_capabilities;
    int require_tests;
    int require_evidence;
    int allow_degraded_optional_capabilities;
} UmiApplicationProductionAcceptanceRule;

UmiApplicationProductionAcceptanceRule
umi_application_production_acceptance_rule_default(void);
UmiStatus umi_application_production_acceptance_rule_validate(
    const UmiApplicationProductionAcceptanceRule *rule);

#ifdef __cplusplus
}
#endif
#endif
