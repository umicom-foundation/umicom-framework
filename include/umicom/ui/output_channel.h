/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/output_channel.h
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
#ifndef UMICOM_UI_OUTPUT_CHANNEL_H
#define UMICOM_UI_OUTPUT_CHANNEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_OUTPUT_CHANNEL_CAPACITY 4096U
#define UMI_UI_OUTPUT_CHANNEL_API_VERSION 1U

/**
 * Represent the ui output channel snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiOutputChannelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char category[128];
    char text[2048];
    uint64_t sequence;
    int visible;
    int preserve;
    uint64_t revision;
} UmiUiOutputChannelSnapshot;

/**
 * Represent the ui output channel registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiOutputChannelRegistry UmiUiOutputChannelRegistry;

/**
 * Initialise ui output channel registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_output_channel_registry_create(UmiUiOutputChannelRegistry **out_registry);
/**
 * Release or reset state held by ui output channel registry so the same storage can be
 * reused safely.
 */
void umi_ui_output_channel_registry_destroy(UmiUiOutputChannelRegistry *registry);
/**
 * Provide the ui output channel registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_output_channel_registry_upsert(UmiUiOutputChannelRegistry *registry, const UmiUiOutputChannelSnapshot *item);
/**
 * Remove ui output channel registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_output_channel_registry_remove(UmiUiOutputChannelRegistry *registry, const char *id);
/**
 * Find ui output channel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_output_channel_registry_find(const UmiUiOutputChannelRegistry *registry, const char *id, UmiUiOutputChannelSnapshot *out_item);
/**
 * Find ui output channel registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_output_channel_registry_at(const UmiUiOutputChannelRegistry *registry, size_t index, UmiUiOutputChannelSnapshot *out_item);
/**
 * Return the number of records represented by ui output channel registry without changing
 * their state.
 */
size_t umi_ui_output_channel_registry_count(const UmiUiOutputChannelRegistry *registry);
/**
 * Provide the ui output channel registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_output_channel_registry_revision(const UmiUiOutputChannelRegistry *registry);
/**
 * Release or reset state held by ui output channel registry so the same storage can be
 * reused safely.
 */
void umi_ui_output_channel_registry_clear(UmiUiOutputChannelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
