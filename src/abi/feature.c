/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/feature.c
 *
 * PURPOSE:
 *   Validate ABI feature metadata before it enters a descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/feature.h"
#include <stddef.h>
UmiStatus umi_abi_feature_validate(const UmiAbiFeature *feature){if(feature==NULL||feature->feature_id==NULL||feature->feature_id[0]=='\0'||feature->version==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
