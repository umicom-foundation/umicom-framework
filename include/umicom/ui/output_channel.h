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

/* BEGINNER NOTE:
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

typedef struct UmiUiOutputChannelRegistry UmiUiOutputChannelRegistry;

UmiStatus umi_ui_output_channel_registry_create(UmiUiOutputChannelRegistry **out_registry);
void umi_ui_output_channel_registry_destroy(UmiUiOutputChannelRegistry *registry);
UmiStatus umi_ui_output_channel_registry_upsert(UmiUiOutputChannelRegistry *registry, const UmiUiOutputChannelSnapshot *item);
UmiStatus umi_ui_output_channel_registry_remove(UmiUiOutputChannelRegistry *registry, const char *id);
UmiStatus umi_ui_output_channel_registry_find(const UmiUiOutputChannelRegistry *registry, const char *id, UmiUiOutputChannelSnapshot *out_item);
UmiStatus umi_ui_output_channel_registry_at(const UmiUiOutputChannelRegistry *registry, size_t index, UmiUiOutputChannelSnapshot *out_item);
size_t umi_ui_output_channel_registry_count(const UmiUiOutputChannelRegistry *registry);
uint64_t umi_ui_output_channel_registry_revision(const UmiUiOutputChannelRegistry *registry);
void umi_ui_output_channel_registry_clear(UmiUiOutputChannelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
