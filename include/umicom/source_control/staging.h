/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/staging.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_SOURCE_CONTROL_STAGING_H
#define UMICOM_SOURCE_CONTROL_STAGING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_STAGING_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_STAGING_API_VERSION 1U

typedef struct UmiSourceControlStagingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char change_id[128];
    char hunk_id[128];
    int staged;
    int partial;
    uint64_t revision;
} UmiSourceControlStagingSnapshot;

typedef struct UmiSourceControlStagingRegistry UmiSourceControlStagingRegistry;

UmiStatus umi_source_control_staging_registry_create(UmiSourceControlStagingRegistry **out_registry);
void umi_source_control_staging_registry_destroy(UmiSourceControlStagingRegistry *registry);
UmiStatus umi_source_control_staging_registry_upsert(UmiSourceControlStagingRegistry *registry, const UmiSourceControlStagingSnapshot *item);
UmiStatus umi_source_control_staging_registry_remove(UmiSourceControlStagingRegistry *registry, const char *id);
UmiStatus umi_source_control_staging_registry_find(const UmiSourceControlStagingRegistry *registry, const char *id, UmiSourceControlStagingSnapshot *out_item);
UmiStatus umi_source_control_staging_registry_at(const UmiSourceControlStagingRegistry *registry, size_t index, UmiSourceControlStagingSnapshot *out_item);
size_t umi_source_control_staging_registry_count(const UmiSourceControlStagingRegistry *registry);
uint64_t umi_source_control_staging_registry_revision(const UmiSourceControlStagingRegistry *registry);
void umi_source_control_staging_registry_clear(UmiSourceControlStagingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
