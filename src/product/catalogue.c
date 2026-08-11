/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/catalogue.c
 *
 * PURPOSE:
 *   Expose the canonical Umicom reference-product catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/catalogue.h"
#include <string.h>
size_t umi_reference_product_count(void){return 5U;}
const UmiProductProfile *umi_reference_product_at(size_t i){switch(i){case 0U:return umi_reference_product_studio();case 1U:return umi_reference_product_designer();case 2U:return umi_reference_product_trader();case 3U:return umi_reference_product_tms();case 4U:return umi_reference_product_media_studio();default:return NULL;}}
const UmiProductProfile *umi_reference_product_find(const char *id){size_t i;if(id==NULL)return NULL;for(i=0U;i<umi_reference_product_count();++i){const UmiProductProfile *p=umi_reference_product_at(i);if(strcmp(p->descriptor->product_id,id)==0)return p;}return NULL;}
