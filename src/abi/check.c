/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/check.c
 *
 * PURPOSE:
 *   Combine descriptor, platform and baseline evidence into one ABI gate result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/check.h"
#include <stddef.h>
UmiStatus umi_abi_check_run(const UmiAbiDescriptor *d,const UmiAbiBaseline *e,const UmiAbiBaseline *a,UmiAbiCheckResult *o){if(o==NULL||d==NULL||e==NULL||a==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->descriptor_valid=umi_abi_descriptor_validate(d)==UMI_STATUS_OK;o->platform_valid=umi_abi_platform_validate(&d->platform)==UMI_STATUS_OK;o->baseline_valid=umi_abi_baseline_matches(e,a);o->passed=o->descriptor_valid&&o->platform_valid&&o->baseline_valid;return UMI_STATUS_OK;}
