/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/trade_publisher.c
 *
 * PURPOSE:
 *   Publish canonical trade contexts from TMS, risk, reconciliation and settlement views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/trade_publisher.h"

#include <string.h>

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
    if (status != UMI_STATUS_OK) return status;
    if (source_panel_id != NULL && source_panel_id[0] != '\0') {
        return umi_context_copy_text(
            payload->identity.source_panel_id,
            sizeof(payload->identity.source_panel_id),
            source_panel_id);
    }
    payload->identity.source_panel_id[0] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_publish_trade(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * trade_id,
    const char * source_system,
    const char * product_type,
    const char * book_id,
    const char * counterparty_id,
    uint64_t version,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    if (host == NULL ||
        context_id == NULL ||
        trade_id == NULL ||
        source_system == NULL ||
        product_type == NULL ||
        book_id == NULL ||
        counterparty_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_TRADE, context_id, "org.umicom.context.trade");
    status = set_source_identity(&payload, host, source_panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_trade_id(&payload.domain.trade, trade_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_source_system(&payload.domain.trade, source_system);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_product_type(&payload.domain.trade, product_type);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_book_id(&payload.domain.trade, book_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_counterparty_id(&payload.domain.trade, counterparty_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_version(&payload.domain.trade, version);
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
