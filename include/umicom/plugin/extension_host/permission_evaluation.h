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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostPermissionEvaluation { uint64_t requested; uint64_t granted; uint64_t denied; int workspace_trusted; int allow_prompt; } UmiPluginExtensionHostPermissionEvaluation;
void umi_plugin_extension_host_permission_evaluation_init(UmiPluginExtensionHostPermissionEvaluation *value);
uint64_t umi_plugin_extension_host_permission_evaluation_missing(const UmiPluginExtensionHostPermissionEvaluation *value);
UmiPluginExtensionHostDecision umi_plugin_extension_host_permission_evaluation_decide(const UmiPluginExtensionHostPermissionEvaluation *value);

#ifdef __cplusplus
}
#endif

#endif
