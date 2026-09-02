/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/configuration.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_CONFIGURATION_H
#define UMICOM_PROJECT_CONFIGURATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_CONFIGURATION_CAPACITY 1024U
#define UMI_PROJECT_CONFIGURATION_API_VERSION 1U

/**
 * Represent the project configuration snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectConfigurationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char build_type[64];
    char toolchain_id[128];
    char platform[128];
    int active;
    uint64_t revision;
} UmiProjectConfigurationSnapshot;

/**
 * Represent the project configuration registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectConfigurationRegistry UmiProjectConfigurationRegistry;

/**
 * Initialise project configuration registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_configuration_registry_create(UmiProjectConfigurationRegistry **out_registry);
/**
 * Release or reset state held by project configuration registry so the same storage can be
 * reused safely.
 */
void umi_project_configuration_registry_destroy(UmiProjectConfigurationRegistry *registry);
/**
 * Provide the project configuration registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_configuration_registry_upsert(UmiProjectConfigurationRegistry *registry, const UmiProjectConfigurationSnapshot *item);
/**
 * Remove project configuration registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_configuration_registry_remove(UmiProjectConfigurationRegistry *registry, const char *id);
/**
 * Find project configuration registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_configuration_registry_find(const UmiProjectConfigurationRegistry *registry, const char *id, UmiProjectConfigurationSnapshot *out_item);
/**
 * Find project configuration registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_configuration_registry_at(const UmiProjectConfigurationRegistry *registry, size_t index, UmiProjectConfigurationSnapshot *out_item);
/**
 * Return the number of records represented by project configuration registry without
 * changing their state.
 */
size_t umi_project_configuration_registry_count(const UmiProjectConfigurationRegistry *registry);
/**
 * Provide the project configuration registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_project_configuration_registry_revision(const UmiProjectConfigurationRegistry *registry);
/**
 * Release or reset state held by project configuration registry so the same storage can be
 * reused safely.
 */
void umi_project_configuration_registry_clear(UmiProjectConfigurationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
