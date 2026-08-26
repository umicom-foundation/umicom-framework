/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_z_order.c
 *
 * PURPOSE:
 *   Validate represent component stacking order independently of renderer implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/z_order.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadZOrder item;CHECK(umi_rad_z_order_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_z_order_is_valid(&item));return 0;}
