/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/z_order.h
 *
 * PURPOSE:
 *   Represent component stacking order independently of renderer implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_Z_ORDER_H
#define UMICOM_DESIGNER_RAD_Z_ORDER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadZOrder {
    char component_id[UMI_RAD_ID_CAPACITY];
    int32_t order;
} UmiRadZOrder;
UmiStatus umi_rad_z_order_init(UmiRadZOrder *item);
int umi_rad_z_order_is_valid(const UmiRadZOrder *item);
#ifdef __cplusplus
}
#endif
#endif
