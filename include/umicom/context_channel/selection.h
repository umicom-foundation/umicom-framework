/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/selection.h
 *
 * PURPOSE:
 *   Define the canonical selection context shared by applications and panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SELECTION_H
#define UMICOM_CONTEXT_CHANNEL_SELECTION_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSelectionContext {
    uint32_t structure_size;
    char selection_id[UMI_CONTEXT_TEXT_CAPACITY];
    char selection_type[96U];
    char primary_id[UMI_CONTEXT_TEXT_CAPACITY];
    char secondary_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t index;
    uint64_t count;
    uint64_t revision;
} UmiSelectionContext;
void umi_selection_context_init(UmiSelectionContext *context);
UmiStatus umi_selection_context_validate(const UmiSelectionContext *context);
UmiStatus umi_selection_context_copy(UmiSelectionContext *destination, const UmiSelectionContext *source);
UmiStatus umi_selection_context_set_selection_id(UmiSelectionContext *context, const char *value);
UmiStatus umi_selection_context_set_selection_type(UmiSelectionContext *context, const char *value);
UmiStatus umi_selection_context_set_primary_id(UmiSelectionContext *context, const char *value);
UmiStatus umi_selection_context_set_secondary_id(UmiSelectionContext *context, const char *value);
UmiStatus umi_selection_context_set_index(UmiSelectionContext *context, uint64_t value);
UmiStatus umi_selection_context_set_count(UmiSelectionContext *context, uint64_t value);
#ifdef __cplusplus
}
#endif
#endif
