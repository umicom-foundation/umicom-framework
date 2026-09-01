/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_hash.c
 *
 * PURPOSE:
 *   Validate abi hash behaviour for the major release hardening train.
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
int main(void){assert(umi_abi_hash_text("umicom")!=0U);assert(umi_abi_hash_text("umicom")==umi_abi_hash_text("umicom"));return 0;}
