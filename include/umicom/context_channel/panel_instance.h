/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/panel_instance.h
 *
 * PURPOSE:
 *   Track runtime panel instances placed into shared layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_PANEL_INSTANCE_H
#define UMICOM_CONTEXT_CHANNEL_PANEL_INSTANCE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_PANEL_INSTANCE_MAX_ITEMS 256U
/**
 * Represent the cross application panel instance data shared with callers of this public
 * contract.
 */
typedef struct UmiCrossApplicationPanelInstance {
    uint32_t structure_size;
    char instance_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char layout_node_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool visible;
    bool active;
    uint64_t last_context_sequence;
    uint64_t revision;
} UmiCrossApplicationPanelInstance;
/**
 * Represent the cross application panel instance store data shared with callers of this
 * public contract.
 */
typedef struct UmiCrossApplicationPanelInstanceStore {
    UmiCrossApplicationPanelInstance items[UMI_CONTEXT_PANEL_INSTANCE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCrossApplicationPanelInstanceStore;
/**
 * Initialise context panel instance from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_panel_instance_init(UmiCrossApplicationPanelInstance *record);
/**
 * Check that context panel instance satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_panel_instance_validate(const UmiCrossApplicationPanelInstance *record);
/**
 * Initialise context panel instance store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_panel_instance_store_init(UmiCrossApplicationPanelInstanceStore *store);
/**
 * Provide the context panel instance store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_panel_instance_store_put(UmiCrossApplicationPanelInstanceStore *store,const UmiCrossApplicationPanelInstance *record);
/**
 * Remove context panel instance store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_panel_instance_store_remove(UmiCrossApplicationPanelInstanceStore *store,const char *identity);
/**
 * Find context panel instance store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiCrossApplicationPanelInstance *umi_context_panel_instance_store_find(UmiCrossApplicationPanelInstanceStore *store,const char *identity);
/**
 * Provide the context panel instance store find const operation used by this module and
 * its client applications.
 */
const UmiCrossApplicationPanelInstance *umi_context_panel_instance_store_find_const(const UmiCrossApplicationPanelInstanceStore *store,const char *identity);
/**
 * Return the number of records represented by context panel instance store without
 * changing their state.
 */
size_t umi_context_panel_instance_store_count(const UmiCrossApplicationPanelInstanceStore *store);
/**
 * Provide the context panel instance store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_panel_instance_store_snapshot(const UmiCrossApplicationPanelInstanceStore *store,UmiCrossApplicationPanelInstance *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
