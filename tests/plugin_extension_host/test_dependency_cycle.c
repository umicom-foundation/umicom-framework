/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_dependency_cycle.c
 *
 * PURPOSE:
 *   Exercise detect cycles in extension dependency topology before activation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/dependency_cycle.h"
int main(void) { UmiPluginExtensionHostDependencyCycle g; umi_plugin_extension_host_dependency_cycle_init(&g,3U); umi_plugin_extension_host_dependency_cycle_add_edge(&g,0U,1U); umi_plugin_extension_host_dependency_cycle_add_edge(&g,1U,2U); if(umi_plugin_extension_host_dependency_cycle_has_cycle(&g)) return 1; umi_plugin_extension_host_dependency_cycle_add_edge(&g,2U,0U); if(!umi_plugin_extension_host_dependency_cycle_has_cycle(&g)) return 2; return 0; }
