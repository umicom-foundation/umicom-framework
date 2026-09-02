/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_dependency_graph.c
 *
 * PURPOSE:
 *   Exercise maintain bounded extension dependency edges and dependency queries.
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
#include "umicom/plugin/extension_host/dependency_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostDependencyGraph g; umi_plugin_extension_host_dependency_graph_init(&g); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_dependency_graph_add(&g,"a","b",0)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_dependency_graph_depends_on(&g,"a","b")||umi_plugin_extension_host_dependency_graph_out_degree(&g,"a")!=1U) return 2; return 0; }
