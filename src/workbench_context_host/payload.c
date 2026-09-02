/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/payload.c
 *
 * PURPOSE:
 *   Construct generic selection payloads and attach typed metadata values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/payload.h"

#include "umicom/workbench_context_link/payload_builder.h"

/*
 * Provide the workbench context host payload add text operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_payload_add_text(
    UmiContextPayload *payload,
    const char *name,
    const char *text)
{
    UmiContextValue value;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (payload == NULL || name == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_value_init(&value, name);
    status = umi_context_value_set_text(&value, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_context_payload_add_value(payload, &value);
}

/*
 * Provide the workbench context host payload add unsigned operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_payload_add_unsigned(
    UmiContextPayload *payload,
    const char *name,
    uint64_t number)
{
    UmiContextValue value;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (payload == NULL || name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_value_init(&value, name);
    status = umi_context_value_set_unsigned(&value, number);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_context_payload_add_value(payload, &value);
}

/*
 * Provide the workbench context host payload from observation operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_payload_from_observation(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const UmiWorkbenchContextHostObservation *observation)
{
    const char *primary;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_payload == NULL || context_id == NULL ||
        application_id == NULL || panel_id == NULL ||
        observation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    primary = observation->document_id[0] != '\0'
        ? observation->document_id
        : (observation->view_container_id[0] != '\0'
            ? observation->view_container_id
            : observation->perspective_id);

    status = umi_workbench_context_link_build_selection(
        out_payload,
        context_id,
        application_id,
        panel_id,
        context_id,
        "workbench",
        primary[0] != '\0' ? primary : "workbench");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (observation->workspace_profile[0] != '\0') {
        status = umi_workbench_context_host_payload_add_text(
            out_payload,
            "workspace-profile",
            observation->workspace_profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (observation->perspective_id[0] != '\0') {
        status = umi_workbench_context_host_payload_add_text(
            out_payload,
            "perspective",
            observation->perspective_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (observation->activity_id[0] != '\0') {
        status = umi_workbench_context_host_payload_add_text(
            out_payload,
            "activity",
            observation->activity_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (observation->view_container_id[0] != '\0') {
        status = umi_workbench_context_host_payload_add_text(
            out_payload,
            "view-container",
            observation->view_container_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_workbench_context_host_payload_add_unsigned(
        out_payload,
        "workbench-revision",
        observation->source_revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_context_payload_refresh_hash(out_payload);
    }
    return status;
}
