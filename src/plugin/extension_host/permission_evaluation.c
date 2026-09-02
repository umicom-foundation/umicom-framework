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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/permission_evaluation.h"
#include <string.h>
/*
 * Initialise plugin extension host permission evaluation from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_permission_evaluation_init(UmiPluginExtensionHostPermissionEvaluation *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL) { memset(value,0,sizeof(*value)); value->allow_prompt=1; } }
/*
 * Provide the plugin extension host permission evaluation missing operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_permission_evaluation_missing(const UmiPluginExtensionHostPermissionEvaluation *value) { return value==NULL?UINT64_MAX:(value->requested & ~value->granted); }
/*
 * Provide the plugin extension host permission evaluation decide operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_permission_evaluation_decide(const UmiPluginExtensionHostPermissionEvaluation *value) { uint64_t missing; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||!value->workspace_trusted||(value->requested&value->denied)!=0U) return UMI_PLUGIN_EXTENSION_HOST_DENY; missing=umi_plugin_extension_host_permission_evaluation_missing(value); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(missing==0U) return UMI_PLUGIN_EXTENSION_HOST_ALLOW; return value->allow_prompt?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; }
