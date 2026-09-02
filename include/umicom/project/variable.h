/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/variable.h
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
#ifndef UMICOM_PROJECT_VARIABLE_H
#define UMICOM_PROJECT_VARIABLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_VARIABLE_CAPACITY 1024U
#define UMI_PROJECT_VARIABLE_API_VERSION 1U

/**
 * Represent the project variable snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectVariableSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char value[1024];
    char scope[64];
    int secret;
    uint64_t revision;
} UmiProjectVariableSnapshot;

/**
 * Represent the project variable registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectVariableRegistry UmiProjectVariableRegistry;

/**
 * Initialise project variable registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_variable_registry_create(UmiProjectVariableRegistry **out_registry);
/**
 * Release or reset state held by project variable registry so the same storage can be
 * reused safely.
 */
void umi_project_variable_registry_destroy(UmiProjectVariableRegistry *registry);
/**
 * Provide the project variable registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_variable_registry_upsert(UmiProjectVariableRegistry *registry, const UmiProjectVariableSnapshot *item);
/**
 * Remove project variable registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_variable_registry_remove(UmiProjectVariableRegistry *registry, const char *id);
/**
 * Find project variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_variable_registry_find(const UmiProjectVariableRegistry *registry, const char *id, UmiProjectVariableSnapshot *out_item);
/**
 * Find project variable registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_variable_registry_at(const UmiProjectVariableRegistry *registry, size_t index, UmiProjectVariableSnapshot *out_item);
/**
 * Return the number of records represented by project variable registry without changing
 * their state.
 */
size_t umi_project_variable_registry_count(const UmiProjectVariableRegistry *registry);
/**
 * Provide the project variable registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_variable_registry_revision(const UmiProjectVariableRegistry *registry);
/**
 * Release or reset state held by project variable registry so the same storage can be
 * reused safely.
 */
void umi_project_variable_registry_clear(UmiProjectVariableRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
