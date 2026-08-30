/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/descriptor.c
 *
 * PURPOSE:
 *   Validate a complete ABI descriptor before compatibility checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/descriptor.h"
#include <stddef.h>
UmiStatus umi_abi_descriptor_validate(const UmiAbiDescriptor *d){size_t i;if(d==NULL||d->structure_size<sizeof(*d)||d->abi_id==NULL||d->abi_id[0]=='\0'||d->abi_version==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_abi_platform_validate(&d->platform)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<d->feature_count;++i)if(umi_abi_feature_validate(&d->features[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<d->symbol_count;++i)if(umi_abi_symbol_validate(&d->symbols[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<d->layout_count;++i)if(umi_abi_struct_layout_validate(&d->layouts[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
