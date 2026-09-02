/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/workstation_conformance.h
 *
 * PURPOSE:
 *   aggregate workstation parity across layout, interaction, accessibility and rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_WORKSTATION_CONFORMANCE_H
#define UMICOM_FRONTEND_CONFORMANCE_WORKSTATION_CONFORMANCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc workstation conformance data shared with callers of this public
 * contract.
 */
typedef struct UmiFcWorkstationConformance { double layout; double interaction; double accessibility; double rendering; double persistence; size_t blockers; } UmiFcWorkstationConformance;
/**
 * Provide the fc workstation conformance score operation used by this module and its
 * client applications.
 */
double umi_fc_workstation_conformance_score(const UmiFcWorkstationConformance *item);
/**
 * Provide the fc workstation conformance outcome operation used by this module and its
 * client applications.
 */
UmiFcOutcome umi_fc_workstation_conformance_outcome(const UmiFcWorkstationConformance *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
