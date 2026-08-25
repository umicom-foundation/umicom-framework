/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/workstation_conformance.h
 *
 * PURPOSE:
 *   aggregate workstation parity across layout, interaction, accessibility and rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcWorkstationConformance { double layout; double interaction; double accessibility; double rendering; double persistence; size_t blockers; } UmiFcWorkstationConformance;
double umi_fc_workstation_conformance_score(const UmiFcWorkstationConformance *item);
UmiFcOutcome umi_fc_workstation_conformance_outcome(const UmiFcWorkstationConformance *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
