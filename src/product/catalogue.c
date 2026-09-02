/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/catalogue.c
 *
 * PURPOSE:
 *   Expose the canonical Umicom reference-product catalogue.
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
#include "umicom/product/catalogue.h"
#include <string.h>
/*
 * Return the number of records represented by reference product without changing their
 * state.
 */
size_t umi_reference_product_count(void){return 5U;}
/*
 * Find reference product while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiProductProfile *umi_reference_product_at(size_t i){/* Select the behaviour associated with the requested command or state value. */ switch(i){case 0U:return umi_reference_product_studio();case 1U:return umi_reference_product_designer();case 2U:return umi_reference_product_trader();case 3U:return umi_reference_product_tms();case 4U:return umi_reference_product_media_studio();default:return NULL;}}
/*
 * Find reference product while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiProductProfile *umi_reference_product_find(const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<umi_reference_product_count();++i){const UmiProductProfile *p=umi_reference_product_at(i);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->descriptor->product_id,id)==0)return p;}return NULL;}
