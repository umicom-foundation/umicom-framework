/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_permission_evaluation.c
 *
 * PURPOSE:
 *   Exercise evaluate requested permissions against grants, denials and workspace trust.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/permission_evaluation.h"
int main(void) { UmiPluginExtensionHostPermissionEvaluation v; umi_plugin_extension_host_permission_evaluation_init(&v); v.workspace_trusted=1; v.requested=UINT64_C(3); v.granted=UINT64_C(1); if(umi_plugin_extension_host_permission_evaluation_decide(&v)!=UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL) return 1; v.granted=UINT64_C(3); if(umi_plugin_extension_host_permission_evaluation_decide(&v)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 2; return 0; }
