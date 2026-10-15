/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/validator.c
 *
 * PURPOSE:
 *   Run a complete SDK-prefix validation using package, header and ABI evidence.
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
#include "umicom/sdk/validator.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/sdk/installation_inspection.h"
#include "umicom/sdk/manifest.h"
/*
 * Provide the sdk validate prefix operation used by this module and its client
 * applications.
 */
/* Retain the process-version check for review. It described the inspecting
 * library, not the requested installation. The active validator requires that
 * installation's passive metadata and never claims compatibility without it. */
#if 0
UmiStatus umi_sdk_validate_prefix(const char *p,const UmiSdkRequirement *r,UmiSdkValidationResult *o){static const UmiSdkComponent components[]={{"framework","Umicom::Framework",1},{"abi","Umicom::abi",1},{"sdk","Umicom::sdk",1},{"product","Umicom::product",1}};UmiSdkManifest m;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||r==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->conformance=(UmiSdkConformance){0U,0U,0U};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_sdk_probe_prefix(p,&o->probe)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;m=umi_sdk_manifest_current(components,sizeof(components)/sizeof(components[0]));o->compatible=umi_sdk_compatible(r,&m);umi_sdk_conformance_record(&o->conformance,o->probe.package_config);umi_sdk_conformance_record(&o->conformance,o->probe.public_header);umi_sdk_conformance_record(&o->conformance,o->probe.libraries);umi_sdk_conformance_record(&o->conformance,o->compatible);o->passed=umi_sdk_conformance_passed(&o->conformance);return UMI_STATUS_OK;}

#endif

UmiStatus umi_sdk_validate_prefix(const char *prefix, const UmiSdkRequirement *requirement,
    UmiSdkValidationResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (prefix == NULL || requirement == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSdkInstallationObservation *observation = calloc(1U, sizeof *observation);
    if (observation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiSdkInstallationInspect(prefix, observation);
    if (status == UMI_STATUS_OK) {
        /* Compatibility uses the selected SDK's declared identity. It is still
         * metadata evidence: compile/link tests establish actual ABI usability. */
        UmiSdkManifest installed = {
            .framework_version = observation->metadata.framework_version,
            .framework_abi = observation->metadata.framework_abi
        };
        out->probe.package_config = observation->package_config && observation->package_targets &&
            observation->package_version;
        out->probe.public_header = observation->header_present;
        out->probe.libraries = observation->base_library_present;
        out->probe.passed = observation->complete;
        out->compatible = umi_sdk_compatible(requirement, &installed);
        umi_sdk_conformance_record(&out->conformance, out->probe.package_config);
        umi_sdk_conformance_record(&out->conformance, out->probe.public_header);
        umi_sdk_conformance_record(&out->conformance, out->probe.libraries);
        umi_sdk_conformance_record(&out->conformance, out->compatible);
        out->passed = umi_sdk_conformance_passed(&out->conformance);
    }
    free(observation);
    return status;
}
