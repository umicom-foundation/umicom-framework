/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/compatibility.c
 *
 * PURPOSE:
 *   Compare an SDK manifest against consumer version and ABI requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/compatibility.h"
#include <stddef.h>
#include "umicom/abi/version.h"
int umi_sdk_compatible(const UmiSdkRequirement *r,const UmiSdkManifest *m){if(r==NULL||umi_sdk_manifest_validate(m)!=UMI_STATUS_OK)return 0;return m->framework_abi>=r->required_abi&&umi_abi_version_is_compatible(r->minimum_version,m->framework_version);}
