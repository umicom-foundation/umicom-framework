/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/baseline.c
 *
 * PURPOSE:
 *   Build and compare the current Framework ABI baseline.
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
#include "umicom/abi/baseline.h"
#include <stddef.h>
/*
 * Provide the abi baseline current operation used by this module and its client
 * applications.
 */
UmiAbiBaseline umi_abi_baseline_current(uint64_t fingerprint){UmiAbiBaseline b={{UMICOM_FRAMEWORK_VERSION_MAJOR,UMICOM_FRAMEWORK_VERSION_MINOR,UMICOM_FRAMEWORK_VERSION_PATCH},UMICOM_FRAMEWORK_ABI_VERSION,fingerprint};return b;}
/*
 * Provide the abi baseline matches operation used by this module and its client
 * applications.
 */
int umi_abi_baseline_matches(const UmiAbiBaseline *e,const UmiAbiBaseline *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||a==NULL)return 0;return e->framework_version.major==a->framework_version.major&&e->framework_abi==a->framework_abi&&e->contract_fingerprint==a->contract_fingerprint;}
