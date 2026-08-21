/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/diagnostics.c
 *
 * PURPOSE:
 *   Compute actionable semantic layout health, complexity, ownership, geometry and recovery readiness for diagnostics panes and quality gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/diagnostics.h"

#include <stdio.h>
#include <string.h>

#include "internal.h"

static bool text_seen(
    char values[][UMI_WORKBENCH_LAYOUT_ID_CAPACITY],
    size_t count,
    const char *value)
{
    size_t index;
    if (!umi_workbench_layout_text_present(value)) {
        return true;
    }
    for (index = 0U; index < count; ++index) {
        if (strcmp(values[index], value) == 0) {
            return true;
        }
    }
    return false;
}

static bool node_offscreen(
    const UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchMonitorTopology *topology)
{
    const UmiWorkbenchMonitor *monitor;

    if (node == NULL || topology == NULL ||
        (node->kind != UMI_WORKBENCH_LAYOUT_NODE_WINDOW &&
         node->kind !=
             UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW)) {
        return false;
    }
    monitor = node->monitor_id[0] != '\0'
        ? umi_workbench_monitor_topology_find(
              topology, node->monitor_id)
        : umi_workbench_monitor_topology_best_for_rect(
              topology, &node->bounds);
    return monitor == NULL ||
           !umi_workbench_layout_rect_intersects(
               &node->bounds, &monitor->work_area);
}

UmiStatus umi_workbench_layout_diagnose(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchMonitorTopology *topology,
    const UmiWorkbenchRecoveryPlan *recovery_plan,
    UmiWorkbenchLayoutHealth *out_health,
    UmiWorkbenchLayoutValidationReport *out_validation)
{
    UmiWorkbenchLayoutValidationOptions options;
    UmiWorkbenchLayoutValidationReport validation;
    char owners[UMI_WORKBENCH_LAYOUT_MAX_NODES]
               [UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char contexts[UMI_WORKBENCH_LAYOUT_MAX_NODES]
                 [UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    size_t owner_count = 0U;
    size_t context_count = 0U;
    size_t index;
    UmiStatus status;

    if (document == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_health, 0, sizeof(*out_health));
    out_health->structure_size = sizeof(*out_health);
    (void)memset(owners, 0, sizeof(owners));
    (void)memset(contexts, 0, sizeof(contexts));

    options =
        umi_workbench_layout_validation_options_default();
    options.validate_hash = true;
    status = umi_workbench_layout_validate(
        document, &options, &validation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_health->valid = validation.valid;
    out_health->connected =
        !umi_workbench_layout_validation_report_has_code(
            &validation,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNREACHABLE_NODE) &&
        !umi_workbench_layout_validation_report_has_code(
            &validation,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CYCLE);
    out_health->hash_valid =
        !umi_workbench_layout_validation_report_has_code(
            &validation,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_HASH_MISMATCH);
    out_health->geometry_safe = true;
    out_health->recovery_ready =
        recovery_plan == NULL ||
        !recovery_plan->recovery_required ||
        recovery_plan->last_safe_sequence > 0U;
    out_health->node_count = document->node_count;
    out_health->warning_count = validation.warning_count;
    out_health->error_count = validation.error_count;
    out_health->content_hash = document->content_hash;

    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node =
            &document->nodes[index];
        size_t depth = umi_workbench_layout_node_depth(
            document, node->node_id);

        if (umi_workbench_layout_node_is_container(node)) {
            out_health->container_count += 1U;
        }
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
            node->kind ==
                UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) {
            out_health->panel_count += 1U;
        }
        if (node->kind ==
            UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW) {
            out_health->floating_window_count += 1U;
        }
        if (depth > out_health->maximum_depth) {
            out_health->maximum_depth = depth;
        }
        if (umi_workbench_layout_text_present(
                node->owner_application_id) &&
            !text_seen(
                owners,
                owner_count,
                node->owner_application_id) &&
            owner_count < UMI_WORKBENCH_LAYOUT_MAX_NODES) {
            (void)umi_workbench_layout_copy_text(
                owners[owner_count],
                sizeof(owners[owner_count]),
                node->owner_application_id,
                false);
            owner_count += 1U;
        }
        if (!text_seen(
                contexts,
                context_count,
                node->context_group_id) &&
            context_count < UMI_WORKBENCH_LAYOUT_MAX_NODES) {
            (void)umi_workbench_layout_copy_text(
                contexts[context_count],
                sizeof(contexts[context_count]),
                node->context_group_id,
                false);
            context_count += 1U;
        }
        if (topology != NULL &&
            node_offscreen(node, topology)) {
            out_health->offscreen_window_count += 1U;
            out_health->geometry_safe = false;
        }
    }

    out_health->application_owner_count = owner_count;
    out_health->context_group_count = context_count;

    (void)snprintf(
        out_health->summary,
        sizeof(out_health->summary),
        "%s layout with %zu nodes, %zu panels, %zu errors and %zu warnings.",
        out_health->valid ? "Valid" : "Invalid",
        out_health->node_count,
        out_health->panel_count,
        out_health->error_count,
        out_health->warning_count);

    if (out_validation != NULL) {
        *out_validation = validation;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_health_format(
    const UmiWorkbenchLayoutHealth *health,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    int written;

    if (health == NULL || out_required == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        buffer,
        capacity,
        "valid=%s connected=%s geometry=%s hash=%s recovery=%s "
        "nodes=%zu containers=%zu panels=%zu floating=%zu "
        "owners=%zu contexts=%zu offscreen=%zu warnings=%zu errors=%zu "
        "depth=%zu hash=%016" PRIx64,
        health->valid ? "yes" : "no",
        health->connected ? "yes" : "no",
        health->geometry_safe ? "safe" : "unsafe",
        health->hash_valid ? "valid" : "invalid",
        health->recovery_ready ? "ready" : "not-ready",
        health->node_count,
        health->container_count,
        health->panel_count,
        health->floating_window_count,
        health->application_owner_count,
        health->context_group_count,
        health->offscreen_window_count,
        health->warning_count,
        health->error_count,
        health->maximum_depth,
        health->content_hash);
    if (written < 0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }

    *out_required = (size_t)written + 1U;
    if (buffer == NULL || capacity < *out_required) {
        if (buffer != NULL && capacity > 0U) {
            buffer[capacity - 1U] = '\0';
        }
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
