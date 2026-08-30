/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_signature_evidence.c
 *
 * PURPOSE:
 *   Exercise capture signature-verification evidence supplied by the Framework security layer.
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
#include "umicom/plugin/extension_host/signature_evidence.h"
int main(void)
{
    UmiPluginExtensionHostSignatureEvidence value; umi_plugin_extension_host_signature_evidence_init(&value);
    if (umi_plugin_extension_host_signature_evidence_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_signature_evidence_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_signature_evidence_fingerprint(&value) == 0U) return 3;
    return 0;
}
