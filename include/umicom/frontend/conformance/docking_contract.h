/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/docking_contract.h
 *
 * PURPOSE:
 *   dock zones, floating, auto-hide and split/tab workstation requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DOCKING_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_DOCKING_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcDockingContract { uint64_t required_features; uint64_t allowed_zones; bool responsive_fallback; } UmiFcDockingContract;
bool umi_fc_docking_contract_validate(const UmiFcDockingContract *item);

#ifdef __cplusplus
}
#endif
#endif
