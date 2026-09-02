/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/bootstrap.c
 *
 * PURPOSE:
 *   Describe the minimal product bootstrap result used by reference consumers.
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
#include "umicom/product/bootstrap.h"
#include <stddef.h>
/* Provide the product bootstrap operation used by this module and its client applications. */
UmiStatus umi_product_bootstrap(const UmiProductProfile *p,UmiProductBootstrapResult *o){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;o->product_id=p->descriptor->product_id;o->required_capabilities=0U;o->enabled_frontends=0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->capability_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->capabilities[i].required)++o->required_capabilities;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->frontend_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->frontends[i].enabled)++o->enabled_frontends;o->ready=o->enabled_frontends>0U;return UMI_STATUS_OK;}
