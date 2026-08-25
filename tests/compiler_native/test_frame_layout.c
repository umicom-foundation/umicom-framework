/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_frame_layout.c
 *
 * PURPOSE:
 *   Regression coverage for allocate aligned stack-frame slots and compute deterministic final frame sizes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/frame_layout.h"
int main(void){UmiNativeFrameLayout l;umi_nc_frame_layout_init(&l,16U);size_t a=0U,b=0U;if(umi_nc_frame_layout_add(&l,1U,8U,8U,&a)!=UMI_STATUS_OK||umi_nc_frame_layout_add(&l,2U,4U,4U,&b)!=UMI_STATUS_OK)return 1;if(a!=0U||b!=8U||umi_nc_frame_layout_finalize(&l)!=16U)return 2;return 0;}
