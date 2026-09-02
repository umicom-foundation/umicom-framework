/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_platform.c
 *
 * PURPOSE:
 *   Validate abi platform behaviour for the major release hardening train.
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
int main(void){UmiAbiPlatform p=umi_abi_platform_current();assert(umi_abi_platform_validate(&p)==UMI_STATUS_OK);assert(umi_abi_platform_compatible(&p,&p));return 0;}
