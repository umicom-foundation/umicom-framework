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

/**
 * Provide the component evidence known mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_evidence_known_mask(void);
/**
 * Provide the component frontend known mask operation used by this module and its client
 * applications.
 */
uint32_t umi_component_frontend_known_mask(void);
/**
 * Provide the component evidence required for status operation used by this module and its
 * client applications.
 */
uint32_t umi_component_evidence_required_for_status(UmiComponentApiStatus status);
/**
 * Provide the component frontends required for status operation used by this module and
 * its client applications.
 */
uint32_t umi_component_frontends_required_for_status(UmiComponentApiStatus status);
/**
 * Provide the component evidence missing operation used by this module and its client
 * applications.
 */
uint32_t umi_component_evidence_missing(const UmiComponentGovernanceRecord *record);
/**
 * Provide the component frontends missing operation used by this module and its client
 * applications.
 */
uint32_t umi_component_frontends_missing(const UmiComponentGovernanceRecord *record);
/**
 * Provide the component evidence is complete operation used by this module and its client
 * applications.
 */
int umi_component_evidence_is_complete(const UmiComponentGovernanceRecord *record);
/**
 * Return the number of records represented by component flag without changing their state.
 */
size_t umi_component_flag_count(uint32_t flags);
/**
 * Add component evidence only after its inputs and available capacity have been checked.
 */
UmiStatus umi_component_evidence_add(UmiComponentGovernanceRecord *record, uint32_t evidence);
/**
 * Add component frontend only after its inputs and available capacity have been checked.
 */
UmiStatus umi_component_frontend_add(UmiComponentGovernanceRecord *record, uint32_t frontend);

#ifdef __cplusplus
}
#endif

#endif
