/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/deprecation.c
 *
 * PURPOSE:
 *   Validate deprecation schedules and determine when an API can be removed.
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
#include "umicom/abi/deprecation.h"
#include <stddef.h>
UmiStatus umi_abi_deprecation_validate(const UmiAbiDeprecation *e){if(e==NULL||e->symbol==NULL||e->symbol[0]=='\0'||e->deprecated_since==0U||e->remove_after<e->deprecated_since)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
int umi_abi_deprecation_removable(const UmiAbiDeprecation *e,uint32_t current){return umi_abi_deprecation_validate(e)==UMI_STATUS_OK&&current>=e->remove_after;}
