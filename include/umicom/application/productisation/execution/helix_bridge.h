/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/helix_bridge.h
 *
 * PURPOSE:
 *   Project completion work items into existing governed Helix action descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_HELIX_BRIDGE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_HELIX_BRIDGE_H
#include "umicom/application/productisation/execution/work_item.h"
#include "umicom/helix/action.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Map a product owner to the governed action kind used by Helix. */
UmiHelixActionKind umi_product_execution_helix_action_kind(
    const UmiProductExecutionWorkItem *item);
/** Build a reviewable Helix action from one completion work item. */
UmiStatus umi_product_execution_helix_action_build(
    const UmiProductExecutionWorkItem *item,
    UmiHelixAction *out_action);
#ifdef __cplusplus
}
#endif
#endif
