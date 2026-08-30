/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_compatibility.c
 *
 * PURPOSE:
 *   Validate abi compatibility behaviour for the major release hardening train.
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
#include "umicom/abi/abi.h"
int main(void){UmiAbiDescriptor d={sizeof(UmiAbiDescriptor),"umicom.test",2U,umi_abi_platform_current(),NULL,0U,NULL,0U,NULL,0U};UmiAbiCompatibility r;assert(umi_abi_compatibility_check(&d,&d,&r)==UMI_STATUS_OK);assert(r.compatible);return 0;}
