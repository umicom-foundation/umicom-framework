/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/permission_evaluation.h
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_EVALUATION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_EVALUATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host permission evaluation data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostPermissionEvaluation { uint64_t requested; uint64_t granted; uint64_t denied; int workspace_trusted; int allow_prompt; } UmiPluginExtensionHostPermissionEvaluation;
/**
 * Initialise plugin extension host permission evaluation from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_permission_evaluation_init(UmiPluginExtensionHostPermissionEvaluation *value);
/**
 * Provide the plugin extension host permission evaluation missing operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_permission_evaluation_missing(const UmiPluginExtensionHostPermissionEvaluation *value);
/**
 * Provide the plugin extension host permission evaluation decide operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_permission_evaluation_decide(const UmiPluginExtensionHostPermissionEvaluation *value);

#ifdef __cplusplus
}
#endif

#endif
