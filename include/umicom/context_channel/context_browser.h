/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_browser.h
 *
 * PURPOSE:
 *   Build immutable browser rows for recent, pinned and active contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BROWSER_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BROWSER_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context browser data shared with callers of this public contract.
 */
typedef struct UmiContextBrowser {
    uint32_t structure_size;
    char filter_text[UMI_CONTEXT_VALUE_CAPACITY];
    char category[UMI_CONTEXT_VALUE_CAPACITY];
    char owner_id[UMI_CONTEXT_VALUE_CAPACITY];
    char workspace_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBrowser;
/**
 * Initialise context browser from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_browser_init(UmiContextBrowser *state);
/**
 * Provide the context browser set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_browser_set_field(UmiContextBrowser *state,size_t field_index,const char *value);
/**
 * Provide the context browser field operation used by this module and its client
 * applications.
 */
const char *umi_context_browser_field(const UmiContextBrowser *state,size_t field_index);
/**
 * Provide the context browser record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_browser_record_success(UmiContextBrowser *state,uint64_t sequence);
/**
 * Provide the context browser record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_browser_record_failure(UmiContextBrowser *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context browser satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_browser_validate(const UmiContextBrowser *state);
/**
 * Provide the context browser covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_browser_covers_sequence(const UmiContextBrowser *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
