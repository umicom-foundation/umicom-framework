/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/source_location.h
 *
 * PURPOSE:
 *   Define the canonical source location context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SOURCE_LOCATION_H
#define UMICOM_CONTEXT_CHANNEL_SOURCE_LOCATION_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSourceLocationContext {
    uint32_t structure_size;
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char file_path[UMI_CONTEXT_PATH_CAPACITY];
    char symbol[UMI_CONTEXT_TEXT_CAPACITY];
    uint32_t line;
    uint32_t column;
    uint32_t selection_length;
    uint64_t revision;
} UmiSourceLocationContext;
void umi_source_location_context_init(UmiSourceLocationContext *context);
UmiStatus umi_source_location_context_validate(const UmiSourceLocationContext *context);
UmiStatus umi_source_location_context_copy(UmiSourceLocationContext *destination, const UmiSourceLocationContext *source);
UmiStatus umi_source_location_context_set_workspace_id(UmiSourceLocationContext *context, const char *value);
UmiStatus umi_source_location_context_set_file_path(UmiSourceLocationContext *context, const char *value);
UmiStatus umi_source_location_context_set_symbol(UmiSourceLocationContext *context, const char *value);
UmiStatus umi_source_location_context_set_line(UmiSourceLocationContext *context, uint32_t value);
UmiStatus umi_source_location_context_set_column(UmiSourceLocationContext *context, uint32_t value);
UmiStatus umi_source_location_context_set_selection_length(UmiSourceLocationContext *context, uint32_t value);
#ifdef __cplusplus
}
#endif
#endif
