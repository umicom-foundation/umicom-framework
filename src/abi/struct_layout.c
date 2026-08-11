/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/struct_layout.c
 *
 * PURPOSE:
 *   Validate and compare public structure size and alignment evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/struct_layout.h"
#include <string.h>
UmiStatus umi_abi_struct_layout_validate(const UmiAbiStructLayout *l){if(l==NULL||l->name==NULL||l->name[0]=='\0'||l->size==0U||l->alignment==0U||l->abi_version==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
int umi_abi_struct_layout_compatible(const UmiAbiStructLayout *r,const UmiAbiStructLayout *a){if(umi_abi_struct_layout_validate(r)!=UMI_STATUS_OK||umi_abi_struct_layout_validate(a)!=UMI_STATUS_OK)return 0;return strcmp(r->name,a->name)==0&&a->size>=r->size&&a->alignment==r->alignment&&a->abi_version>=r->abi_version;}
