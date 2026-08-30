/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/account_publisher.c
 *
 * PURPOSE:
 *   Publish canonical account contexts from banking, trading, operations and risk views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/account_publisher.h"

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

UmiStatus umi_workbench_context_host_publish_account(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * account_id,
    const char * organisation_id,
    const char * book_id,
    const char * currency,
    const char * account_type,
    const char * environment,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    if (host == NULL ||
        context_id == NULL ||
        account_id == NULL ||
        organisation_id == NULL ||
        book_id == NULL ||
        currency == NULL ||
        account_type == NULL ||
        environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_ACCOUNT, context_id, "org.umicom.context.account");
    status = set_source_identity(&payload, host, source_panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_account_id(&payload.domain.account, account_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_organisation_id(&payload.domain.account, organisation_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_book_id(&payload.domain.account, book_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_currency(&payload.domain.account, currency);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_account_type(&payload.domain.account, account_type);
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_environment(&payload.domain.account, environment);
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
