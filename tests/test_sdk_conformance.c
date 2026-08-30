/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_conformance.c
 *
 * PURPOSE:
 *   Validate sdk conformance behaviour for the major release hardening train.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/sdk/sdk.h"
int main(void){UmiSdkConformance c={0};umi_sdk_conformance_record(&c,1);umi_sdk_conformance_record(&c,1);assert(umi_sdk_conformance_passed(&c));return 0;}
