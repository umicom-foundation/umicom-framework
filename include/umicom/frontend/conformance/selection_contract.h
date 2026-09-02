/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/selection_contract.h
 *
 * PURPOSE:
 *   single, multiple and range selection semantics for list, tree, grid and editor surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SELECTION_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_SELECTION_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc selection contract data shared with callers of this public contract.
 */
typedef struct UmiFcSelectionContract { uint64_t required_modes; bool keyboard_extend; bool preserve_on_refresh; } UmiFcSelectionContract;
/**
 * Check that fc selection contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_selection_contract_validate(const UmiFcSelectionContract *item);

#ifdef __cplusplus
}
#endif
#endif
