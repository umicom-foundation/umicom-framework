/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/memory_store.h
 *
 * PURPOSE:
 *   Provide an in-memory persistence adapter for context sessions and bookmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_MEMORY_STORE_H
#define UMICOM_CONTEXT_CHANNEL_MEMORY_STORE_H
#include "umicom/context_channel/registry.h"
#include "umicom/context_channel/router.h"
#include "umicom/context_channel/metrics.h"
#include "umicom/context_channel/panel.h"
#include "umicom/context_channel/panel_instance.h"
#include "umicom/context_channel/group_member.h"
#include "umicom/context_channel/link_group.h"
#include "umicom/context_channel/history_entry.h"
#include "umicom/context_channel/diagnostic.h"
#include "umicom/context_channel/bookmark.h"
#include "umicom/context_channel/session.h"
#include "umicom/context_channel/capability.h"
#include "umicom/context_channel/filter_rule.h"
#include "umicom/context_channel/transformer_rule.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_MEMORY_STORE_NAME "memory-store"
/**
 * Represent the context memory store state data shared with callers of this public
 * contract.
 */
typedef struct UmiContextMemoryStoreState {
    uint64_t revision;
    uint64_t operation_count;
    UmiStatus last_status;
    char last_message[UMI_CONTEXT_VALUE_CAPACITY];
} UmiContextMemoryStoreState;
/**
 * Initialise context memory store state from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_memory_store_state_init(UmiContextMemoryStoreState *state);
/**
 * Provide the context memory store state record operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_memory_store_state_record(UmiContextMemoryStoreState *state,UmiStatus status,const char *message);
/**
 * Return the number of records represented by context memory store operation without
 * changing their state.
 */
uint64_t umi_context_memory_store_operation_count(const UmiContextMemoryStoreState *state);
#ifdef __cplusplus
}
#endif
#endif
