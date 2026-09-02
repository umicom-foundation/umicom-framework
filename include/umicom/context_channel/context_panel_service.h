/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_panel_service.h
 *
 * PURPOSE:
 *   Coordinate panel registration, mounting and context binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_PANEL_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_PANEL_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context panel service data shared with callers of this public contract.
 */
typedef struct UmiContextPanelService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char panel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char instance_id[UMI_CONTEXT_VALUE_CAPACITY];
    char layout_node_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextPanelService;
/**
 * Initialise context panel service from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_panel_service_init(UmiContextPanelService *state);
/**
 * Provide the context panel service set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_panel_service_set_field(UmiContextPanelService *state,size_t field_index,const char *value);
/**
 * Provide the context panel service field operation used by this module and its client
 * applications.
 */
const char *umi_context_panel_service_field(const UmiContextPanelService *state,size_t field_index);
/**
 * Provide the context panel service record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_panel_service_record_success(UmiContextPanelService *state,uint64_t sequence);
/**
 * Provide the context panel service record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_panel_service_record_failure(UmiContextPanelService *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context panel service satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_panel_service_validate(const UmiContextPanelService *state);
/**
 * Provide the context panel service covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_panel_service_covers_sequence(const UmiContextPanelService *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
