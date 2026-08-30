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
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_plugin_extension_host_dependency_constraint_init(UmiPluginExtensionHostDependencyConstraint *value);
UmiStatus umi_plugin_extension_host_dependency_constraint_configure(UmiPluginExtensionHostDependencyConstraint *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
UmiStatus umi_plugin_extension_host_dependency_constraint_validate(const UmiPluginExtensionHostDependencyConstraint *value);
uint64_t umi_plugin_extension_host_dependency_constraint_fingerprint(const UmiPluginExtensionHostDependencyConstraint *value);

#ifdef __cplusplus
}
#endif

#endif
