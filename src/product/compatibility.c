/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/compatibility.c
 *
 * PURPOSE:
 *   Check whether a product profile can run against the current Framework capability catalogue.
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
#include "umicom/product/compatibility.h"
#include <stddef.h>
#include "umicom/runtime/capability_catalogue.h"
/*
 * Provide the product compatibility check operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_compatibility_check(const UmiProductProfile *p,UmiProductCompatibility *o){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;o->required_capabilities=0U;o->available_capabilities=0U;o->missing_capabilities=0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->capability_count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->capabilities[i].required)++o->required_capabilities;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_framework_capability_catalogue_find(p->capabilities[i].capability_id)!=NULL)++o->available_capabilities;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->capabilities[i].required)++o->missing_capabilities;}o->compatible=o->missing_capabilities==0U;return UMI_STATUS_OK;}
