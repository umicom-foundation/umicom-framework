/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/manifest_validation.c
 *
 * PURPOSE:
 *   Validate extension manifest evidence before installation or activation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/manifest_validation.h"
#include <string.h>
void umi_plugin_extension_host_manifest_validation_init(UmiPluginExtensionHostManifestValidation *result) { if(result!=NULL) memset(result,0,sizeof(*result)); }
UmiStatus umi_plugin_extension_host_manifest_validation_record(UmiPluginExtensionHostManifestValidation *result,int id_valid,int version_valid,int entry_valid,int permissions_valid,int checksum_present,int signature_present) { if(result==NULL) return UMI_STATUS_INVALID_ARGUMENT; result->id_valid=id_valid!=0; result->version_valid=version_valid!=0; result->entry_valid=entry_valid!=0; result->permissions_valid=permissions_valid!=0; result->checksum_present=checksum_present!=0; result->signature_present=signature_present!=0; result->issue_count=(uint32_t)(!result->id_valid)+(uint32_t)(!result->version_valid)+(uint32_t)(!result->entry_valid)+(uint32_t)(!result->permissions_valid); return UMI_STATUS_OK; }
int umi_plugin_extension_host_manifest_validation_ready(const UmiPluginExtensionHostManifestValidation *result) { return result!=NULL && result->issue_count==0U && result->checksum_present && result->signature_present; }
