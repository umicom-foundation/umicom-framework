/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/validation.c
 *
 * PURPOSE:
 *   Validate product identifiers, branding, capabilities and profile structure.
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
#include "umicom/product/validation.h"
#include <stddef.h>
/*
 * Perform product validation through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_product_validation_run(const UmiProductProfile *p,UmiProductValidation *o){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->descriptor_valid=p!=NULL&&umi_product_descriptor_validate(p->descriptor)==UMI_STATUS_OK;o->profile_valid=p!=NULL&&umi_product_profile_validate(p)==UMI_STATUS_OK;o->capabilities_valid=1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->capability_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_product_capability_validate(&p->capabilities[i])!=UMI_STATUS_OK)o->capabilities_valid=0;o->passed=o->descriptor_valid&&o->profile_valid&&o->capabilities_valid;return UMI_STATUS_OK;}
