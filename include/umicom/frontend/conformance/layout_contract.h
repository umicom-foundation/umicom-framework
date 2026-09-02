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

/**
 * Represent the fc layout contract data shared with callers of this public contract.
 */
typedef struct UmiFcLayoutContract { size_t panel_count; size_t split_count; size_t tab_count; size_t floating_count; uint64_t required_features; } UmiFcLayoutContract;
/**
 * Provide the fc layout contract satisfied operation used by this module and its client
 * applications.
 */
bool umi_fc_layout_contract_satisfied(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual);
/**
 * Provide the fc layout contract score operation used by this module and its client
 * applications.
 */
double umi_fc_layout_contract_score(const UmiFcLayoutContract *required,const UmiFcLayoutContract *actual);

#ifdef __cplusplus
}
#endif
#endif
