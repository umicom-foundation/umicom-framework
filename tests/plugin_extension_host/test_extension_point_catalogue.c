/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_extension_point_catalogue.c
 *
 * PURPOSE:
 *   Exercise catalogue Framework extension points available to extensions.
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
#include "umicom/plugin/extension_host/extension_point_catalogue.h"
int main(void)
{
    UmiPluginExtensionHostExtensionPointCatalogue c; umi_plugin_extension_host_extension_point_catalogue_init(&c);
    if (umi_plugin_extension_host_extension_point_catalogue_add(&c,"a",1U,5U,11U)!=UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_extension_point_catalogue_add(&c,"b",2U,2U,22U)!=UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_extension_point_catalogue_find(&c,"a")==0 || umi_plugin_extension_host_extension_point_catalogue_count(&c)!=2U) return 3;
    if (umi_plugin_extension_host_extension_point_catalogue_remove(&c,"a")!=UMI_STATUS_OK || umi_plugin_extension_host_extension_point_catalogue_count(&c)!=1U) return 4;
    return 0;
}
