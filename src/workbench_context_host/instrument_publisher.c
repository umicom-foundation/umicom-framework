/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/instrument_publisher.c
 *
 * PURPOSE:
 *   Publish canonical instrument contexts from watchlists, charts, order entry and market-data views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/instrument_publisher.h"

#include <string.h>

/*
 * Provide the set source identity operation used by this module and its client
 * applications.
 */
static UmiStatus set_source_identity(
    UmiContextPayload *payload,
    const UmiWorkbenchContextHost *host,
    const char *source_panel_id)
{
    UmiStatus status;
    status = umi_context_copy_text(
        payload->identity.source_application_id,
        sizeof(payload->identity.source_application_id),
        host->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_panel_id != NULL && source_panel_id[0] != '\0') {
        return umi_context_copy_text(
            payload->identity.source_panel_id,
            sizeof(payload->identity.source_panel_id),
            source_panel_id);
    }
    payload->identity.source_panel_id[0] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host publish instrument operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_publish_instrument(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * instrument_id,
    const char * symbol,
    const char * venue,
    const char * currency,
    const char * asset_class,
    const char * contract_id,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL ||
        context_id == NULL ||
        instrument_id == NULL ||
        symbol == NULL ||
        venue == NULL ||
        currency == NULL ||
        asset_class == NULL ||
        contract_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_INSTRUMENT, context_id, "org.umicom.context.instrument");
    status = set_source_identity(&payload, host, source_panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_instrument_id(&payload.domain.instrument, instrument_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_symbol(&payload.domain.instrument, symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_venue(&payload.domain.instrument, venue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_currency(&payload.domain.instrument, currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_asset_class(&payload.domain.instrument, asset_class);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_contract_id(&payload.domain.instrument, contract_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
