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

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/descriptor.h"
#include <stddef.h>
/* Check that abi descriptor satisfies its contract before another service relies on it. */
UmiStatus umi_abi_descriptor_validate(const UmiAbiDescriptor *d){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->structure_size<sizeof(*d)||d->abi_id==NULL||d->abi_id[0]=='\0'||d->abi_version==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_abi_platform_validate(&d->platform)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<d->feature_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_abi_feature_validate(&d->features[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<d->symbol_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_abi_symbol_validate(&d->symbols[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<d->layout_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_abi_struct_layout_validate(&d->layouts[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
