/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_compatibility.c
 *
 * PURPOSE:
 *   Validate sdk compatibility behaviour for the major release hardening train.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include "umicom/sdk/sdk.h"
int main(void){UmiSdkManifest m=umi_sdk_manifest_current(NULL,0U);UmiSdkRequirement r={{0U,5U,0U},UMICOM_FRAMEWORK_ABI_VERSION};assert(umi_sdk_compatible(&r,&m));return 0;}
