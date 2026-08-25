/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_trust_record.c
 *
 * PURPOSE:
 *   Exercise capture one durable trust decision and its evidence revision.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/trust_record.h"
int main(void)
{
    UmiPluginExtensionHostTrustRecord value; umi_plugin_extension_host_trust_record_init(&value);
    if (umi_plugin_extension_host_trust_record_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_trust_record_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_trust_record_fingerprint(&value) == 0U) return 3;
    return 0;
}
