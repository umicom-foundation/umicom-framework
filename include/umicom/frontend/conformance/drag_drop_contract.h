/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/drag_drop_contract.h
 *
 * PURPOSE:
 *   semantic drag/drop operation, keyboard alternative and docking affordance requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_DRAG_DROP_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc drag drop contract data shared with callers of this public contract.
 */
typedef struct UmiFcDragDropContract { uint64_t required_ops; bool keyboard_alternative; bool visual_preview; } UmiFcDragDropContract;
/**
 * Check that fc drag drop contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_drag_drop_contract_validate(const UmiFcDragDropContract *item);

#ifdef __cplusplus
}
#endif
#endif
