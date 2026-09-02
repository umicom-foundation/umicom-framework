/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/suite.c
 *
 * PURPOSE:
 *   Compose independent products into an Umicom suite without merging their private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/suite.h"
#include <string.h>
/* Check that product suite satisfies its contract before another service relies on it. */
UmiStatus umi_product_suite_validate(const UmiProductSuite *s){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->suite_id==NULL||s->suite_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->product_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_product_profile_validate(s->products[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/* Find product suite while leaving the underlying catalogue or model owned by this module. */
const UmiProductProfile *umi_product_suite_find(const UmiProductSuite *s,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->product_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->products[i]->descriptor->product_id,id)==0)return s->products[i];return NULL;}
