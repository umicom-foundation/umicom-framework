/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/template.h
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
#ifndef UMICOM_PROJECT_TEMPLATE_H
#define UMICOM_PROJECT_TEMPLATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TEMPLATE_CAPACITY 1024U
#define UMI_PROJECT_TEMPLATE_API_VERSION 1U

/**
 * Represent the project template snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectTemplateSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char category[128];
    char description[512];
    char source_uri[1024];
    char language[64];
    char frontends[256];
    int trusted;
    uint64_t revision;
} UmiProjectTemplateSnapshot;

/**
 * Represent the project template registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectTemplateRegistry UmiProjectTemplateRegistry;

/**
 * Initialise project template registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_template_registry_create(UmiProjectTemplateRegistry **out_registry);
/**
 * Release or reset state held by project template registry so the same storage can be
 * reused safely.
 */
void umi_project_template_registry_destroy(UmiProjectTemplateRegistry *registry);
/**
 * Provide the project template registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_template_registry_upsert(UmiProjectTemplateRegistry *registry, const UmiProjectTemplateSnapshot *item);
/**
 * Remove project template registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_template_registry_remove(UmiProjectTemplateRegistry *registry, const char *id);
/**
 * Find project template registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_template_registry_find(const UmiProjectTemplateRegistry *registry, const char *id, UmiProjectTemplateSnapshot *out_item);
/**
 * Find project template registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_template_registry_at(const UmiProjectTemplateRegistry *registry, size_t index, UmiProjectTemplateSnapshot *out_item);
/**
 * Return the number of records represented by project template registry without changing
 * their state.
 */
size_t umi_project_template_registry_count(const UmiProjectTemplateRegistry *registry);
/**
 * Provide the project template registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_template_registry_revision(const UmiProjectTemplateRegistry *registry);
/**
 * Release or reset state held by project template registry so the same storage can be
 * reused safely.
 */
void umi_project_template_registry_clear(UmiProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
