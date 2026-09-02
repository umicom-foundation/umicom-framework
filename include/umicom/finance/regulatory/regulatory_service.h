/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/regulatory_service.h
 *
 * PURPOSE:
 *   Provide aggregate readiness for Framework-owned regulatory reporting and governance services.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_SERVICE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the regulatory service data shared with callers of this public contract.
 */
typedef struct UmiRegulatoryService { int taxonomy_ready; int governance_ready; int controls_ready; int submission_ready; size_t registered_authorities; size_t active_templates; } UmiRegulatoryService;
/* Initialise conservative service state with all readiness gates closed. */
void umi_reg_regulatory_service_init(UmiRegulatoryService *service);
/* Update explicit subsystem readiness without hiding failed gates. */
UmiStatus umi_reg_regulatory_service_set_readiness(UmiRegulatoryService *service,int taxonomy_ready,int governance_ready,int controls_ready,int submission_ready);
/* Return one only when every regulatory subsystem gate is ready and reference data exists. */
int umi_reg_regulatory_service_ready(const UmiRegulatoryService *service);

#ifdef __cplusplus
}
#endif

#endif
