/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_dependency_resolution.c
 *
 * PURPOSE:
 *   Exercise evaluate whether required extension dependencies are present and compatible.
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
#include "umicom/plugin/extension_host/dependency_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostDependencyResolution r; umi_plugin_extension_host_dependency_resolution_init(&r); umi_plugin_extension_host_dependency_resolution_record(&r,1,1,1); umi_plugin_extension_host_dependency_resolution_record(&r,0,0,0); /* Apply this operation only while the related capability or state is available. */ if(!umi_plugin_extension_host_dependency_resolution_ready(&r)||umi_plugin_extension_host_dependency_resolution_missing_required(&r)!=0U) return 1; umi_plugin_extension_host_dependency_resolution_record(&r,1,0,0); /* Apply this operation only while the related capability or state is available. */ if(umi_plugin_extension_host_dependency_resolution_ready(&r)||umi_plugin_extension_host_dependency_resolution_missing_required(&r)!=1U) return 2; return 0; }
