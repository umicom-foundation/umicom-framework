/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_signing_identity.c
 *
 * PURPOSE:
 *   Exercise describe a signing identity and key evidence without owning cryptographic keys.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/signing_identity.h"
int main(void)
{
    UmiPluginExtensionHostSigningIdentity value; umi_plugin_extension_host_signing_identity_init(&value);
    if (umi_plugin_extension_host_signing_identity_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_signing_identity_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_signing_identity_fingerprint(&value) == 0U) return 3;
    return 0;
}
