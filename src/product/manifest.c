/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/manifest.c
 *
 * PURPOSE:
 *   Create a compact human-readable product manifest for generated repositories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/manifest.h"
#include <stdio.h>
UmiStatus umi_product_manifest_format(const UmiProductProfile *p,char *b,size_t cap){int n;if(umi_product_profile_validate(p)!=UMI_STATUS_OK||b==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(b,cap,"product_id: %s\nname: %s\nvendor: %s\ncapabilities: %zu\nfrontends: %zu\n",p->descriptor->product_id,p->descriptor->branding.display_name,p->descriptor->branding.vendor,p->capability_count,p->frontend_count);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
