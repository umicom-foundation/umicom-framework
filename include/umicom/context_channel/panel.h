/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/panel.h
 *
 * PURPOSE:
 *   Register reusable cross-application panel contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PANEL_H
#define UMICOM_CONTEXT_CHANNEL_PANEL_H
#include "umicom/context_channel/payload.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PANEL_MAX_ITEMS 256U
/**
 * Represent the cross application panel data shared with callers of this public contract.
 */
typedef struct UmiCrossApplicationPanel {
    uint32_t structure_size;
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char title[UMI_CONTEXT_TEXT_CAPACITY];
    char component_id[UMI_CONTEXT_TEXT_CAPACITY];
    char default_channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool singleton;
    bool context_aware;
    bool enabled;
    uint64_t revision;
} UmiCrossApplicationPanel;
/**
 * Represent the cross application panel store data shared with callers of this public
 * contract.
 */
typedef struct UmiCrossApplicationPanelStore {
    UmiCrossApplicationPanel items[UMI_CONTEXT_PANEL_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCrossApplicationPanelStore;
/**
 * Initialise context panel from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_panel_init(UmiCrossApplicationPanel *record);
/**
 * Check that context panel satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_panel_validate(const UmiCrossApplicationPanel *record);
/**
 * Initialise context panel store from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_panel_store_init(UmiCrossApplicationPanelStore *store);
/**
 * Provide the context panel store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_panel_store_put(UmiCrossApplicationPanelStore *store,const UmiCrossApplicationPanel *record);
/**
 * Remove context panel store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_panel_store_remove(UmiCrossApplicationPanelStore *store,const char *identity);
/**
 * Find context panel store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiCrossApplicationPanel *umi_context_panel_store_find(UmiCrossApplicationPanelStore *store,const char *identity);
/**
 * Provide the context panel store find const operation used by this module and its client
 * applications.
 */
const UmiCrossApplicationPanel *umi_context_panel_store_find_const(const UmiCrossApplicationPanelStore *store,const char *identity);
/**
 * Return the number of records represented by context panel store without changing their
 * state.
 */
size_t umi_context_panel_store_count(const UmiCrossApplicationPanelStore *store);
/**
 * Provide the context panel store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_panel_store_snapshot(const UmiCrossApplicationPanelStore *store,UmiCrossApplicationPanel *out_records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_panel_archive_encode(const UmiCrossApplicationPanel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_panel_archive_decode(const void *bytes, size_t byte_count,
    UmiCrossApplicationPanel *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCrossApplicationPanel object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_panel_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCrossApplicationPanel *workspace, UmiCrossApplicationPanel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_panel_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCrossApplicationPanel *expected, UmiCrossApplicationPanel *value);


#ifdef __cplusplus
}
#endif
#endif
