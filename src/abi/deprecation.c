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
/* Check that abi deprecation satisfies its contract before another service relies on it. */
UmiStatus umi_abi_deprecation_validate(const UmiAbiDeprecation *e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||e->symbol==NULL||e->symbol[0]=='\0'||e->deprecated_since==0U||e->remove_after<e->deprecated_since)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the abi deprecation removable operation used by this module and its client
 * applications.
 */
int umi_abi_deprecation_removable(const UmiAbiDeprecation *e,uint32_t current){return umi_abi_deprecation_validate(e)==UMI_STATUS_OK&&current>=e->remove_after;}
