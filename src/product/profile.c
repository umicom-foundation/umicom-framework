/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/profile.c
 *
 * PURPOSE:
 *   Combine capabilities, features and frontends into a product composition profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/profile.h"
#include <stddef.h>
UmiStatus umi_product_profile_validate(const UmiProductProfile *p){size_t i;if(p==NULL||umi_product_descriptor_validate(p->descriptor)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<p->capability_count;++i)if(umi_product_capability_validate(&p->capabilities[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<p->feature_count;++i)if(umi_product_feature_validate(&p->features[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
