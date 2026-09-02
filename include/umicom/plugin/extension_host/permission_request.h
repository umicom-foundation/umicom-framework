/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/permission_request.h
 *
 * PURPOSE:
 *   Describe one extension permission request and its declared scope.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_REQUEST_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PERMISSION_REQUEST_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host permission request data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostPermissionRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostPermissionRequest;

/**
 * Initialise plugin extension host permission request from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_permission_request_init(UmiPluginExtensionHostPermissionRequest *value);
/**
 * Provide the plugin extension host permission request configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_permission_request_configure(UmiPluginExtensionHostPermissionRequest *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host permission request satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_plugin_extension_host_permission_request_validate(const UmiPluginExtensionHostPermissionRequest *value);
/**
 * Provide the plugin extension host permission request fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_permission_request_fingerprint(const UmiPluginExtensionHostPermissionRequest *value);

#ifdef __cplusplus
}
#endif

#endif
