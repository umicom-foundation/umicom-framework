/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/selection.h
 *
 * PURPOSE:
 *   Define the canonical selection context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SELECTION_H
#define UMICOM_CONTEXT_CHANNEL_SELECTION_H
#include "umicom/context_channel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the selection context data shared with callers of this public contract.
 */
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
/**
 * Initialise selection context from caller-provided values so later operations receive a
 * known state.
 */
void umi_selection_context_init(UmiSelectionContext *context);
/**
 * Check that selection context satisfies its contract before another service relies on it.
 */
UmiStatus umi_selection_context_validate(const UmiSelectionContext *context);
/**
 * Copy selection context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_selection_context_copy(UmiSelectionContext *destination, const UmiSelectionContext *source);
/**
 * Provide the selection context set selection id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_id(UmiSelectionContext *context, const char *value);
/**
 * Provide the selection context set selection type operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_selection_type(UmiSelectionContext *context, const char *value);
/**
 * Provide the selection context set primary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_primary_id(UmiSelectionContext *context, const char *value);
/**
 * Provide the selection context set secondary id operation used by this module and its
 * client applications.
 */
UmiStatus umi_selection_context_set_secondary_id(UmiSelectionContext *context, const char *value);
/**
 * Provide the selection context set index operation used by this module and its client
 * applications.
 */
UmiStatus umi_selection_context_set_index(UmiSelectionContext *context, uint64_t value);
/**
 * Return the number of records represented by selection context set without changing their
 * state.
 */
UmiStatus umi_selection_context_set_count(UmiSelectionContext *context, uint64_t value);
#ifdef __cplusplus
}
#endif
#endif
