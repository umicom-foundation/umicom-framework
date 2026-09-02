/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_version.c
 *
 * PURPOSE:
 *   Validate abi version behaviour for the major release hardening train.
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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVersion a={1,2,0},b={1,3,0};assert(umi_abi_version_compare(a,b)<0);assert(umi_abi_version_is_compatible(a,b));return 0;}
