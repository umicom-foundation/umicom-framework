/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/feature.c
 *
 * PURPOSE:
 *   Describe a named product feature independently from its user interface.
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
#include "umicom/product/feature.h"
#include <stddef.h>
UmiStatus umi_product_feature_validate(const UmiProductFeature *f){if(f==NULL||f->feature_id==NULL||f->feature_id[0]=='\0'||f->description==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
