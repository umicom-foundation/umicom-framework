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

typedef struct UmiFcDragDropContract { uint64_t required_ops; bool keyboard_alternative; bool visual_preview; } UmiFcDragDropContract;
bool umi_fc_drag_drop_contract_validate(const UmiFcDragDropContract *item);

#ifdef __cplusplus
}
#endif
#endif
