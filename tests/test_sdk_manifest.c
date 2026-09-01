/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_manifest.c
 *
 * PURPOSE:
 *   Validate sdk manifest behaviour for the major release hardening train.
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
#include "umicom/sdk/sdk.h"
int main(void){UmiSdkComponent c={"framework","Umicom::Framework",1};UmiSdkManifest m=umi_sdk_manifest_current(&c,1U);assert(umi_sdk_manifest_validate(&m)==UMI_STATUS_OK);return 0;}
