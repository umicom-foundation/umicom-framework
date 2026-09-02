/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_descriptor.c
 *
 * PURPOSE:
 *   Validate abi descriptor behaviour for the major release hardening train.
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
int main(void){UmiAbiDescriptor d={sizeof(UmiAbiDescriptor),"umicom.test",2U,umi_abi_platform_current(),NULL,0U,NULL,0U,NULL,0U};assert(umi_abi_descriptor_validate(&d)==UMI_STATUS_OK);assert(umi_abi_contract_fingerprint(&d)!=0U);return 0;}
