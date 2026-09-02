/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_type_descriptor.c
 *
 * PURPOSE:
 *   Regression coverage for describe portable native compiler types, sizes, alignments and derived pointer/array relationships.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/type_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeTypeDescriptor i,p,a;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_type_primitive(&i,1U,UMI_NC_TYPE_INT,4U,4U)!=UMI_STATUS_OK)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_type_pointer(&p,2U,1U,8U)!=UMI_STATUS_OK)return 2;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_type_array(&a,3U,1U,4U,4U,10U)!=UMI_STATUS_OK||a.size!=40U)return 3;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_nc_type_compatible(&i,&i))return 4;return 0;}
