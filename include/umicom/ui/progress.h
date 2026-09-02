/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/progress.h
 *
 * PURPOSE:
 *   Define an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
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

/**
 * Represent the ui progress snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the ui progress registry data shared with callers of this public contract.
 */
typedef struct UmiUiProgressRegistry UmiUiProgressRegistry;

/**
 * Initialise ui progress registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_progress_registry_create(UmiUiProgressRegistry **out_registry);
/**
 * Release or reset state held by ui progress registry so the same storage can be reused
 * safely.
 */
void umi_ui_progress_registry_destroy(UmiUiProgressRegistry *registry);
/**
 * Provide the ui progress registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_progress_registry_upsert(UmiUiProgressRegistry *registry, const UmiUiProgressSnapshot *item);
/**
 * Remove ui progress registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_progress_registry_remove(UmiUiProgressRegistry *registry, const char *id);
/**
 * Find ui progress registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_progress_registry_find(const UmiUiProgressRegistry *registry, const char *id, UmiUiProgressSnapshot *out_item);
/**
 * Find ui progress registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_progress_registry_at(const UmiUiProgressRegistry *registry, size_t index, UmiUiProgressSnapshot *out_item);
/**
 * Return the number of records represented by ui progress registry without changing their
 * state.
 */
size_t umi_ui_progress_registry_count(const UmiUiProgressRegistry *registry);
/**
 * Provide the ui progress registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_progress_registry_revision(const UmiUiProgressRegistry *registry);
/**
 * Release or reset state held by ui progress registry so the same storage can be reused
 * safely.
 */
void umi_ui_progress_registry_clear(UmiUiProgressRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
