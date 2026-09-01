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

typedef struct UmiProjectTemplateRegistry UmiProjectTemplateRegistry;

UmiStatus umi_project_template_registry_create(UmiProjectTemplateRegistry **out_registry);
void umi_project_template_registry_destroy(UmiProjectTemplateRegistry *registry);
UmiStatus umi_project_template_registry_upsert(UmiProjectTemplateRegistry *registry, const UmiProjectTemplateSnapshot *item);
UmiStatus umi_project_template_registry_remove(UmiProjectTemplateRegistry *registry, const char *id);
UmiStatus umi_project_template_registry_find(const UmiProjectTemplateRegistry *registry, const char *id, UmiProjectTemplateSnapshot *out_item);
UmiStatus umi_project_template_registry_at(const UmiProjectTemplateRegistry *registry, size_t index, UmiProjectTemplateSnapshot *out_item);
size_t umi_project_template_registry_count(const UmiProjectTemplateRegistry *registry);
uint64_t umi_project_template_registry_revision(const UmiProjectTemplateRegistry *registry);
void umi_project_template_registry_clear(UmiProjectTemplateRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
