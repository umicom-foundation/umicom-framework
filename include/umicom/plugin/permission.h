/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/permission.h
 *
 * PURPOSE:
 *   Evaluate plug-in permission requests against product grants before a module
 *   can be activated.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_PERMISSION_H
#define UMICOM_PLUGIN_PERMISSION_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"
#include "umicom/security/permission.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPluginPermissionDecision { size_t requested; size_t granted; size_t denied; char first_denied[UMI_PLUGIN_ITEM_CAPACITY]; } UmiPluginPermissionDecision;
UmiStatus umi_plugin_permissions_evaluate(const UmiPluginManifest *manifest,
                                          const UmiPermissionSet *grants,
                                          UmiPluginPermissionDecision *out_decision);
#ifdef __cplusplus
}
#endif

#endif
