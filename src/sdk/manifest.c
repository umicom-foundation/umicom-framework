/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/manifest.c
 *
 * PURPOSE:
 *   Describe the installed Umicom Framework SDK version, ABI and component inventory.
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
#include "umicom/sdk/manifest.h"
#include <stddef.h>
#include <stdint.h>
/*
 * Provide the sdk manifest current operation used by this module and its client
 * applications.
 */
UmiSdkManifest umi_sdk_manifest_current(const UmiSdkComponent *c,size_t n){UmiSdkManifest m={{UMICOM_FRAMEWORK_VERSION_MAJOR,UMICOM_FRAMEWORK_VERSION_MINOR,UMICOM_FRAMEWORK_VERSION_PATCH},UMICOM_FRAMEWORK_ABI_VERSION,c,n};return m;}
/* Check that sdk manifest satisfies its contract before another service relies on it. */
/* The earlier validator formed component addresses before confirming the counted inventory had storage. The active validator checks the array first while retaining empty inventories and component rules. Keep the old implementation for review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_sdk_manifest_validate(const UmiSdkManifest *m){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||m->framework_abi==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->component_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_sdk_component_validate(&m->components[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
#endif
UmiStatus umi_sdk_manifest_validate(const UmiSdkManifest *manifest)
{
    /* Empty metadata inventories remain valid. A nonempty inventory needs
     * caller-owned contiguous storage and a representable byte extent. */
    if (manifest == NULL || manifest->framework_abi == 0U ||
        (manifest->component_count != 0U && manifest->components == NULL) ||
        manifest->component_count > SIZE_MAX / sizeof *manifest->components)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < manifest->component_count; ++index) {
        if (umi_sdk_component_validate(&manifest->components[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
