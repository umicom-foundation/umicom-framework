/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/task.h
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

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_TASK_H
#define UMICOM_PROJECT_TASK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TASK_CAPACITY 1024U
#define UMI_PROJECT_TASK_API_VERSION 1U

typedef struct UmiProjectTaskSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char label[256];
    char command[1024];
    char working_directory[1024];
    char group[128];
    int default_task;
    int background;
    int enabled;
    uint64_t revision;
} UmiProjectTaskSnapshot;

typedef struct UmiProjectTaskRegistry UmiProjectTaskRegistry;

UmiStatus umi_project_task_registry_create(UmiProjectTaskRegistry **out_registry);
void umi_project_task_registry_destroy(UmiProjectTaskRegistry *registry);
UmiStatus umi_project_task_registry_upsert(UmiProjectTaskRegistry *registry, const UmiProjectTaskSnapshot *item);
UmiStatus umi_project_task_registry_remove(UmiProjectTaskRegistry *registry, const char *id);
UmiStatus umi_project_task_registry_find(const UmiProjectTaskRegistry *registry, const char *id, UmiProjectTaskSnapshot *out_item);
UmiStatus umi_project_task_registry_at(const UmiProjectTaskRegistry *registry, size_t index, UmiProjectTaskSnapshot *out_item);
size_t umi_project_task_registry_count(const UmiProjectTaskRegistry *registry);
uint64_t umi_project_task_registry_revision(const UmiProjectTaskRegistry *registry);
void umi_project_task_registry_clear(UmiProjectTaskRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
