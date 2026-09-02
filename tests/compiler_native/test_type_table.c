/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_type_table.c
 *
 * PURPOSE:
 *   Regression coverage for maintain canonical type descriptors with stable ids and structural lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_table.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeTypeTable t;umi_nc_type_table_init(&t);UmiNativeTypeDescriptor i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_type_primitive(&i,1U,UMI_NC_TYPE_INT,4U,4U)!=UMI_STATUS_OK)return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_type_table_add(&t,&i)!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_type_table_find(&t,1U)==NULL||umi_nc_type_table_find_compatible(&t,&i)==NULL)return 3;return 0;}
