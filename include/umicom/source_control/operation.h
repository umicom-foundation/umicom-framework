/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/operation.h
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

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_SOURCE_CONTROL_OPERATION_H
#define UMICOM_SOURCE_CONTROL_OPERATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_OPERATION_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_OPERATION_API_VERSION 1U

/**
 * Represent the source control operation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlOperationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char kind[64];
    char description[512];
    char detail[1024];
    int state;
    int cancellable;
    uint64_t revision;
} UmiSourceControlOperationSnapshot;

/**
 * Represent the source control operation registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlOperationRegistry UmiSourceControlOperationRegistry;

/**
 * Initialise source control operation registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_operation_registry_create(UmiSourceControlOperationRegistry **out_registry);
/**
 * Release or reset state held by source control operation registry so the same storage can
 * be reused safely.
 */
void umi_source_control_operation_registry_destroy(UmiSourceControlOperationRegistry *registry);
/**
 * Provide the source control operation registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_control_operation_registry_upsert(UmiSourceControlOperationRegistry *registry, const UmiSourceControlOperationSnapshot *item);
/**
 * Remove source control operation registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_source_control_operation_registry_remove(UmiSourceControlOperationRegistry *registry, const char *id);
/**
 * Find source control operation registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_operation_registry_find(const UmiSourceControlOperationRegistry *registry, const char *id, UmiSourceControlOperationSnapshot *out_item);
/**
 * Find source control operation registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_operation_registry_at(const UmiSourceControlOperationRegistry *registry, size_t index, UmiSourceControlOperationSnapshot *out_item);
/**
 * Return the number of records represented by source control operation registry without
 * changing their state.
 */
size_t umi_source_control_operation_registry_count(const UmiSourceControlOperationRegistry *registry);
/**
 * Provide the source control operation registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_operation_registry_revision(const UmiSourceControlOperationRegistry *registry);
/**
 * Release or reset state held by source control operation registry so the same storage can
 * be reused safely.
 */
void umi_source_control_operation_registry_clear(UmiSourceControlOperationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
