/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/permission_evaluation.c
 *
 * PURPOSE:
 *   Evaluate requested permissions against grants, denials and workspace trust.
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
#include <string.h>
void umi_plugin_extension_host_permission_evaluation_init(UmiPluginExtensionHostPermissionEvaluation *value) { if(value!=NULL) { memset(value,0,sizeof(*value)); value->allow_prompt=1; } }
uint64_t umi_plugin_extension_host_permission_evaluation_missing(const UmiPluginExtensionHostPermissionEvaluation *value) { return value==NULL?UINT64_MAX:(value->requested & ~value->granted); }
UmiPluginExtensionHostDecision umi_plugin_extension_host_permission_evaluation_decide(const UmiPluginExtensionHostPermissionEvaluation *value) { uint64_t missing; if(value==NULL||!value->workspace_trusted||(value->requested&value->denied)!=0U) return UMI_PLUGIN_EXTENSION_HOST_DENY; missing=umi_plugin_extension_host_permission_evaluation_missing(value); if(missing==0U) return UMI_PLUGIN_EXTENSION_HOST_ALLOW; return value->allow_prompt?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; }
