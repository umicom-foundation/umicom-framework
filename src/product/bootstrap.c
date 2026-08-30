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

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/bootstrap.h"
#include <stddef.h>
UmiStatus umi_product_bootstrap(const UmiProductProfile *p,UmiProductBootstrapResult *o){size_t i;if(o==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;o->product_id=p->descriptor->product_id;o->required_capabilities=0U;o->enabled_frontends=0U;for(i=0U;i<p->capability_count;++i)if(p->capabilities[i].required)++o->required_capabilities;for(i=0U;i<p->frontend_count;++i)if(p->frontends[i].enabled)++o->enabled_frontends;o->ready=o->enabled_frontends>0U;return UMI_STATUS_OK;}
