/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_trust_store.c
 *
 * PURPOSE:
 *   Exercise maintain bounded trust decisions by extension or publisher identity.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/trust_store.h"
int main(void)
{
    UmiPluginExtensionHostTrustStore c; umi_plugin_extension_host_trust_store_init(&c);
    if (umi_plugin_extension_host_trust_store_add(&c,"a",1U,5U,11U)!=UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_trust_store_add(&c,"b",2U,2U,22U)!=UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_trust_store_find(&c,"a")==0 || umi_plugin_extension_host_trust_store_count(&c)!=2U) return 3;
    if (umi_plugin_extension_host_trust_store_remove(&c,"a")!=UMI_STATUS_OK || umi_plugin_extension_host_trust_store_count(&c)!=1U) return 4;
    return 0;
}
