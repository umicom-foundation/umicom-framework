/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_manifest_evidence.c
 *
 * PURPOSE:
 *   Exercise capture immutable evidence derived from a parsed extension manifest.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/manifest_evidence.h"
int main(void)
{
    UmiPluginExtensionHostManifestEvidence value; umi_plugin_extension_host_manifest_evidence_init(&value);
    if (umi_plugin_extension_host_manifest_evidence_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_manifest_evidence_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_manifest_evidence_fingerprint(&value) == 0U) return 3;
    return 0;
}
