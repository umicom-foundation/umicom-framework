/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_abi_catalogue.c
 *
 * PURPOSE:
 *   Validate abi catalogue behaviour for the major release hardening train.
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
int main(void){UmiAbiCatalogue *c=NULL;UmiAbiDescriptor d={sizeof(UmiAbiDescriptor),"umicom.test",2U,umi_abi_platform_current(),NULL,0U,NULL,0U,NULL,0U};assert(umi_abi_catalogue_create(&c)==UMI_STATUS_OK);assert(umi_abi_catalogue_register(c,&d)==UMI_STATUS_OK);assert(umi_abi_catalogue_find(c,"umicom.test")==&d);umi_abi_catalogue_destroy(c);return 0;}
