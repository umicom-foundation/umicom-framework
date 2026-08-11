/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/validator.c
 *
 * PURPOSE:
 *   Run a complete SDK-prefix validation using package, header and ABI evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/validator.h"
#include <stddef.h>
#include "umicom/sdk/manifest.h"
UmiStatus umi_sdk_validate_prefix(const char *p,const UmiSdkRequirement *r,UmiSdkValidationResult *o){static const UmiSdkComponent components[]={{"framework","Umicom::Framework",1},{"abi","Umicom::abi",1},{"sdk","Umicom::sdk",1},{"product","Umicom::product",1}};UmiSdkManifest m;if(p==NULL||r==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;o->conformance=(UmiSdkConformance){0U,0U,0U};if(umi_sdk_probe_prefix(p,&o->probe)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;m=umi_sdk_manifest_current(components,sizeof(components)/sizeof(components[0]));o->compatible=umi_sdk_compatible(r,&m);umi_sdk_conformance_record(&o->conformance,o->probe.package_config);umi_sdk_conformance_record(&o->conformance,o->probe.public_header);umi_sdk_conformance_record(&o->conformance,o->probe.libraries);umi_sdk_conformance_record(&o->conformance,o->compatible);o->passed=umi_sdk_conformance_passed(&o->conformance);return UMI_STATUS_OK;}
