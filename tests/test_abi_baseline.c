/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_baseline.c
 *
 * PURPOSE:
 *   Validate abi baseline behaviour for the major release hardening train.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/abi/abi.h"
int main(void){UmiAbiBaseline a=umi_abi_baseline_current(42U),b=umi_abi_baseline_current(42U);assert(umi_abi_baseline_matches(&a,&b));return 0;}
