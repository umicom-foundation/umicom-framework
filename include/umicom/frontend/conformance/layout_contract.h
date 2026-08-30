/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_contract.h
 *
 * PURPOSE:
 *   toolkit-neutral layout structure expectations used by frontend equivalence checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_LAYOUT_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_LAYOUT_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcLayoutContract { size_t panel_count; size_t split_count; size_t tab_count; size_t floating_count; uint64_t required_features; } UmiFcLayoutContract;
bool umi_fc_layout_contract_satisfied(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual);
double umi_fc_layout_contract_score(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual);

#ifdef __cplusplus
}
#endif
#endif
