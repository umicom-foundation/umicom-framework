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
/* Provide the sdk compatible operation used by this module and its client applications. */
/* The earlier direct predicate accepted a zero ABI requirement that the consumer contract rejected. Centralized validation now rejects that incomplete declaration while preserving the comparison policy. Retain the prior predicate for review. The former implementation is retained for owner review. */
#if 0
int umi_sdk_compatible(const UmiSdkRequirement *r,const UmiSdkManifest *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||umi_sdk_manifest_validate(m)!=UMI_STATUS_OK)return 0;return m->framework_abi>=r->required_abi&&umi_abi_version_is_compatible(r->minimum_version,m->framework_version);}
#endif
UmiStatus umi_sdk_requirement_validate(const UmiSdkRequirement *requirement)
{
    return requirement != NULL && requirement->required_abi != 0U
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

int umi_sdk_compatible(const UmiSdkRequirement *requirement, const UmiSdkManifest *manifest)
{
    /* Apply the same declaration contract for direct and consumer callers.
     * Preserve the established minimum-ABI and same-major version policy;
     * neither this predicate nor the declarations establish binary usability. */
    if (umi_sdk_requirement_validate(requirement) != UMI_STATUS_OK ||
        umi_sdk_manifest_validate(manifest) != UMI_STATUS_OK)
        return 0;
    return manifest->framework_abi >= requirement->required_abi &&
        umi_abi_version_is_compatible(requirement->minimum_version, manifest->framework_version);
}
