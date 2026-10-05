/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/dependency_constraint.h
 *
 * PURPOSE:
 *   Describe one extension dependency version/capability constraint.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_CONSTRAINT_H
#define UMICOM_PLUGIN_EXTENSION_HOST_DEPENDENCY_CONSTRAINT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host dependency constraint data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostDependencyConstraint {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostDependencyConstraint;

/**
 * Initialise plugin extension host dependency constraint from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_dependency_constraint_init(UmiPluginExtensionHostDependencyConstraint *value);
/**
 * Provide the plugin extension host dependency constraint configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_dependency_constraint_configure(UmiPluginExtensionHostDependencyConstraint *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host dependency constraint satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_plugin_extension_host_dependency_constraint_validate(const UmiPluginExtensionHostDependencyConstraint *value);
/**
 * Provide the plugin extension host dependency constraint fingerprint operation used by
 * this module and its client applications.
 */
uint64_t umi_plugin_extension_host_dependency_constraint_fingerprint(const UmiPluginExtensionHostDependencyConstraint *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_plugin_extension_host_dependency_constraint_archive_encode(const UmiPluginExtensionHostDependencyConstraint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_plugin_extension_host_dependency_constraint_archive_decode(const void *bytes, size_t byte_count,
    UmiPluginExtensionHostDependencyConstraint *value);

#ifdef __cplusplus
}
#endif

#endif
