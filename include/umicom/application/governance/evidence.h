/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/governance/evidence.h
 *
 * PURPOSE:
 *   Define reusable evidence calculations used to promote component contracts
 *   from planned foundations to stable, verified Framework building blocks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_GOVERNANCE_EVIDENCE_H
#define UMICOM_APPLICATION_GOVERNANCE_EVIDENCE_H

#include <stddef.h>

#include "umicom/application/governance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t umi_component_evidence_known_mask(void);
uint32_t umi_component_frontend_known_mask(void);
uint32_t umi_component_evidence_required_for_status(UmiComponentApiStatus status);
uint32_t umi_component_frontends_required_for_status(UmiComponentApiStatus status);
uint32_t umi_component_evidence_missing(const UmiComponentGovernanceRecord *record);
uint32_t umi_component_frontends_missing(const UmiComponentGovernanceRecord *record);
int umi_component_evidence_is_complete(const UmiComponentGovernanceRecord *record);
size_t umi_component_flag_count(uint32_t flags);
UmiStatus umi_component_evidence_add(UmiComponentGovernanceRecord *record, uint32_t evidence);
UmiStatus umi_component_frontend_add(UmiComponentGovernanceRecord *record, uint32_t frontend);

#ifdef __cplusplus
}
#endif

#endif
