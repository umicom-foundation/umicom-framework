/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_type_table.c
 *
 * PURPOSE:
 *   Regression coverage for maintain canonical type descriptors with stable ids and structural lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_table.h"
int main(void){UmiNativeTypeTable t;umi_nc_type_table_init(&t);UmiNativeTypeDescriptor i;if(umi_nc_type_primitive(&i,1U,UMI_NC_TYPE_INT,4U,4U)!=UMI_STATUS_OK)return 1;if(umi_nc_type_table_add(&t,&i)!=UMI_STATUS_OK)return 2;if(umi_nc_type_table_find(&t,1U)==NULL||umi_nc_type_table_find_compatible(&t,&i)==NULL)return 3;return 0;}
