/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_ruler.c
 *
 * PURPOSE:
 *   Validate describe design-time rulers and origin offsets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/ruler.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadRuler item;CHECK(umi_rad_ruler_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_ruler_is_valid(&item));return 0;}
