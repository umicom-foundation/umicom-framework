/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/suite.c
 *
 * PURPOSE:
 *   Compose independent products into an Umicom suite without merging their private state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/suite.h"
#include <string.h>
UmiStatus umi_product_suite_validate(const UmiProductSuite *s){size_t i;if(s==NULL||s->suite_id==NULL||s->suite_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->product_count;++i)if(umi_product_profile_validate(s->products[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
const UmiProductProfile *umi_product_suite_find(const UmiProductSuite *s,const char *id){size_t i;if(s==NULL||id==NULL)return NULL;for(i=0U;i<s->product_count;++i)if(strcmp(s->products[i]->descriptor->product_id,id)==0)return s->products[i];return NULL;}
