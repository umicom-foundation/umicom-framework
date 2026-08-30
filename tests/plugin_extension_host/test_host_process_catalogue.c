/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_process_catalogue.c
 *
 * PURPOSE:
 *   Exercise maintain bounded isolated host-process state.
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
#include "umicom/plugin/extension_host/host_process_catalogue.h"
int main(void)
{
    UmiPluginExtensionHostHostProcessCatalogue c; umi_plugin_extension_host_host_process_catalogue_init(&c);
    if (umi_plugin_extension_host_host_process_catalogue_add(&c,"a",1U,5U,11U)!=UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_host_process_catalogue_add(&c,"b",2U,2U,22U)!=UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_host_process_catalogue_find(&c,"a")==0 || umi_plugin_extension_host_host_process_catalogue_count(&c)!=2U) return 3;
    if (umi_plugin_extension_host_host_process_catalogue_remove(&c,"a")!=UMI_STATUS_OK || umi_plugin_extension_host_host_process_catalogue_count(&c)!=1U) return 4;
    return 0;
}
