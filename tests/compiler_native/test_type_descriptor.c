/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_type_descriptor.c
 *
 * PURPOSE:
 *   Regression coverage for describe portable native compiler types, sizes, alignments and derived pointer/array relationships.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_descriptor.h"
int main(void){UmiNativeTypeDescriptor i,p,a;if(umi_nc_type_primitive(&i,1U,UMI_NC_TYPE_INT,4U,4U)!=UMI_STATUS_OK)return 1;if(umi_nc_type_pointer(&p,2U,1U,8U)!=UMI_STATUS_OK)return 2;if(umi_nc_type_array(&a,3U,1U,4U,4U,10U)!=UMI_STATUS_OK||a.size!=40U)return 3;if(!umi_nc_type_compatible(&i,&i))return 4;return 0;}
