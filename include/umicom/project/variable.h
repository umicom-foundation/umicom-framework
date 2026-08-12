/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/variable.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiProjectVariableRegistry UmiProjectVariableRegistry;

UmiStatus umi_project_variable_registry_create(UmiProjectVariableRegistry **out_registry);
void umi_project_variable_registry_destroy(UmiProjectVariableRegistry *registry);
UmiStatus umi_project_variable_registry_upsert(UmiProjectVariableRegistry *registry, const UmiProjectVariableSnapshot *item);
UmiStatus umi_project_variable_registry_remove(UmiProjectVariableRegistry *registry, const char *id);
UmiStatus umi_project_variable_registry_find(const UmiProjectVariableRegistry *registry, const char *id, UmiProjectVariableSnapshot *out_item);
UmiStatus umi_project_variable_registry_at(const UmiProjectVariableRegistry *registry, size_t index, UmiProjectVariableSnapshot *out_item);
size_t umi_project_variable_registry_count(const UmiProjectVariableRegistry *registry);
uint64_t umi_project_variable_registry_revision(const UmiProjectVariableRegistry *registry);
void umi_project_variable_registry_clear(UmiProjectVariableRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
