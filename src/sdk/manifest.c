/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/manifest.c
 *
 * PURPOSE:
 *   Describe the installed Umicom Framework SDK version, ABI and component inventory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/manifest.h"
#include <stddef.h>
UmiSdkManifest umi_sdk_manifest_current(const UmiSdkComponent *c,size_t n){UmiSdkManifest m={{UMICOM_FRAMEWORK_VERSION_MAJOR,UMICOM_FRAMEWORK_VERSION_MINOR,UMICOM_FRAMEWORK_VERSION_PATCH},UMICOM_FRAMEWORK_ABI_VERSION,c,n};return m;}
UmiStatus umi_sdk_manifest_validate(const UmiSdkManifest *m){size_t i;if(m==NULL||m->framework_abi==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<m->component_count;++i)if(umi_sdk_component_validate(&m->components[i])!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
