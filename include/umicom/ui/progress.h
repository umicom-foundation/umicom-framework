/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/progress.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_UI_PROGRESS_H
#define UMICOM_UI_PROGRESS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PROGRESS_CAPACITY 4096U
#define UMI_UI_PROGRESS_API_VERSION 1U

typedef struct UmiUiProgressSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char detail[512];
    double fraction;
    int state;
    int cancellable;
    int indeterminate;
    uint64_t revision;
} UmiUiProgressSnapshot;

typedef struct UmiUiProgressRegistry UmiUiProgressRegistry;

UmiStatus umi_ui_progress_registry_create(UmiUiProgressRegistry **out_registry);
void umi_ui_progress_registry_destroy(UmiUiProgressRegistry *registry);
UmiStatus umi_ui_progress_registry_upsert(UmiUiProgressRegistry *registry, const UmiUiProgressSnapshot *item);
UmiStatus umi_ui_progress_registry_remove(UmiUiProgressRegistry *registry, const char *id);
UmiStatus umi_ui_progress_registry_find(const UmiUiProgressRegistry *registry, const char *id, UmiUiProgressSnapshot *out_item);
UmiStatus umi_ui_progress_registry_at(const UmiUiProgressRegistry *registry, size_t index, UmiUiProgressSnapshot *out_item);
size_t umi_ui_progress_registry_count(const UmiUiProgressRegistry *registry);
uint64_t umi_ui_progress_registry_revision(const UmiUiProgressRegistry *registry);
void umi_ui_progress_registry_clear(UmiUiProgressRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
