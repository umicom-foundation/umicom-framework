/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/commit.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
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
#ifndef UMICOM_SOURCE_CONTROL_COMMIT_H
#define UMICOM_SOURCE_CONTROL_COMMIT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_COMMIT_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_COMMIT_API_VERSION 1U

typedef struct UmiSourceControlCommitSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char hash[128];
    char author[256];
    char email[256];
    char subject[512];
    uint64_t timestamp;
    int head;
    uint64_t revision;
} UmiSourceControlCommitSnapshot;

typedef struct UmiSourceControlCommitRegistry UmiSourceControlCommitRegistry;

UmiStatus umi_source_control_commit_registry_create(UmiSourceControlCommitRegistry **out_registry);
void umi_source_control_commit_registry_destroy(UmiSourceControlCommitRegistry *registry);
UmiStatus umi_source_control_commit_registry_upsert(UmiSourceControlCommitRegistry *registry, const UmiSourceControlCommitSnapshot *item);
UmiStatus umi_source_control_commit_registry_remove(UmiSourceControlCommitRegistry *registry, const char *id);
UmiStatus umi_source_control_commit_registry_find(const UmiSourceControlCommitRegistry *registry, const char *id, UmiSourceControlCommitSnapshot *out_item);
UmiStatus umi_source_control_commit_registry_at(const UmiSourceControlCommitRegistry *registry, size_t index, UmiSourceControlCommitSnapshot *out_item);
size_t umi_source_control_commit_registry_count(const UmiSourceControlCommitRegistry *registry);
uint64_t umi_source_control_commit_registry_revision(const UmiSourceControlCommitRegistry *registry);
void umi_source_control_commit_registry_clear(UmiSourceControlCommitRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
