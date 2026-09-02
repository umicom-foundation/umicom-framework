/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/profile.c
 *
 * PURPOSE:
 *   Combine capabilities, features and frontends into a product composition profile.
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
#include "umicom/product/profile.h"
#include <stddef.h>
/* Check that product profile satisfies its contract before another service relies on it. */
UmiStatus umi_product_profile_validate(const UmiProductProfile *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||umi_product_descriptor_validate(p->descriptor)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->capability_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_product_capability_validate(&p->capabilities[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->feature_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_product_feature_validate(&p->features[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
