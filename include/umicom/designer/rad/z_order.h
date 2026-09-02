/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/z_order.h
 *
 * PURPOSE:
 *   Represent component stacking order independently of renderer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_Z_ORDER_H
#define UMICOM_DESIGNER_RAD_Z_ORDER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad zorder data shared with callers of this public contract.
 */
typedef struct UmiRadZOrder {
    char component_id[UMI_RAD_ID_CAPACITY];
    int32_t order;
} UmiRadZOrder;
/**
 * Initialise rad z order from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_z_order_init(UmiRadZOrder *item);
/**
 * Check that rad z order satisfies its contract before another service relies on it.
 */
int umi_rad_z_order_is_valid(const UmiRadZOrder *item);
#ifdef __cplusplus
}
#endif
#endif
