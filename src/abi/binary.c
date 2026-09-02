/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/binary.c
 *
 * PURPOSE:
 *   Validate and compare native binary metadata before loading or deployment.
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
#include "umicom/abi/binary.h"
#include <stddef.h>
/* Check that abi binary satisfies its contract before another service relies on it. */
UmiStatus umi_abi_binary_validate(const UmiAbiBinary *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||b->name==NULL||b->name[0]=='\0'||b->abi_version==0U||umi_abi_platform_validate(&b->platform)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the abi binary compatible operation used by this module and its client
 * applications.
 */
int umi_abi_binary_compatible(const UmiAbiBinary *c,const UmiAbiBinary *p){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_abi_binary_validate(c)!=UMI_STATUS_OK||umi_abi_binary_validate(p)!=UMI_STATUS_OK)return 0;return c->abi_version<=p->abi_version&&umi_abi_platform_compatible(&c->platform,&p->platform)&&(c->contract_fingerprint==0U||c->contract_fingerprint==p->contract_fingerprint);}
