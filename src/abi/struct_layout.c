/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/struct_layout.c
 *
 * PURPOSE:
 *   Validate and compare public structure size and alignment evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/struct_layout.h"
#include <string.h>
/* Check that abi struct layout satisfies its contract before another service relies on it. */
UmiStatus umi_abi_struct_layout_validate(const UmiAbiStructLayout *l){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||l->name==NULL||l->name[0]=='\0'||l->size==0U||l->alignment==0U||l->abi_version==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the abi struct layout compatible operation used by this module and its client
 * applications.
 */
int umi_abi_struct_layout_compatible(const UmiAbiStructLayout *r,const UmiAbiStructLayout *a){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_abi_struct_layout_validate(r)!=UMI_STATUS_OK||umi_abi_struct_layout_validate(a)!=UMI_STATUS_OK)return 0;return strcmp(r->name,a->name)==0&&a->size>=r->size&&a->alignment==r->alignment&&a->abi_version>=r->abi_version;}
