/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_platform.c
 *
 * PURPOSE:
 *   Validate abi platform behaviour for the major release hardening train.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/abi/abi.h"
int main(void){UmiAbiPlatform p=umi_abi_platform_current();assert(umi_abi_platform_validate(&p)==UMI_STATUS_OK);assert(umi_abi_platform_compatible(&p,&p));return 0;}
