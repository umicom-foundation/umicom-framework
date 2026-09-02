/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/service.c
 *
 * PURPOSE:
 *   Implement colour-group definition, panel binding, typed publication, routing, pinning and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/service.h"

#include <stdio.h>
#include <string.h>

/* Provide the mode can follow operation used by this module and its client applications. */
static bool mode_can_follow(UmiWorkbenchContextLinkMode mode)
{
    return mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW ||
           mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
}

/* Provide the mode can publish operation used by this module and its client applications. */
static bool mode_can_publish(UmiWorkbenchContextLinkMode mode)
{
    return mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH ||
           mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
}

/* Provide the build deliveries operation used by this module and its client applications. */
static UmiStatus build_deliveries(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *source_panel_id,
    UmiContextKind kind,
    UmiContextChannelColour colour,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_deliveries == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_deliveries, 0, sizeof(*out_deliveries));
    out_deliveries->sequence = service->sequence;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < service->bindings.count; ++index) {
        const UmiWorkbenchContextLinkPanelBinding *binding = &service->bindings.items[index];
        UmiWorkbenchContextLinkDelivery *delivery;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(binding->group_id, group_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!mode_can_follow(binding->mode)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (binding->state == UMI_WORKBENCH_CONTEXT_LINK_STATE_SUSPENDED) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (binding->context_kind != kind && binding->context_kind != UMI_CONTEXT_KIND_GENERIC) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (source_panel_id != NULL && source_panel_id[0] != '\0' &&
            strcmp(binding->panel_id, source_panel_id) == 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_deliveries->count >= UMI_WORKBENCH_CONTEXT_LINK_MAX_DELIVERIES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        delivery = &out_deliveries->items[out_deliveries->count++];
        (void)umi_workbench_context_link_copy_text(
            delivery->binding_id, sizeof(delivery->binding_id), binding->binding_id);
        (void)umi_workbench_context_link_copy_text(
            delivery->panel_id, sizeof(delivery->panel_id), binding->panel_id);
        (void)umi_workbench_context_link_copy_text(
            delivery->application_id, sizeof(delivery->application_id), binding->application_id);
        (void)umi_workbench_context_link_copy_text(
            delivery->group_id, sizeof(delivery->group_id), group_id);
        delivery->context_kind = kind;
        delivery->colour = colour;
        delivery->sequence = service->sequence;
    }
    service->metrics.delivery_count += (uint64_t)out_deliveries->count;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context link service from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_service_init(
    UmiWorkbenchContextLinkService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    memset(service, 0, sizeof(*service));
    umi_workbench_context_link_group_catalogue_init(&service->groups);
    umi_workbench_context_link_binding_registry_init(&service->bindings);
    umi_workbench_context_link_active_context_store_init(&service->active);
    umi_workbench_context_link_history_store_init(&service->history);
    umi_workbench_context_link_pin_store_init(&service->pins);
    service->sequence = 1U;
    service->revision = 1U;
}

/*
 * Release or reset state held by workbench context link service so the same storage can be
 * reused safely.
 */
void umi_workbench_context_link_service_destroy(
    UmiWorkbenchContextLinkService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_workbench_context_link_group_catalogue_destroy(&service->groups);
    umi_workbench_context_link_binding_registry_destroy(&service->bindings);
    umi_workbench_context_link_active_context_store_destroy(&service->active);
    umi_workbench_context_link_history_store_destroy(&service->history);
    umi_workbench_context_link_pin_store_destroy(&service->pins);
    memset(service, 0, sizeof(*service));
}

/*
 * Provide the workbench context link service define group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_define_group(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *title,
    UmiContextChannelColour colour,
    uint64_t allowed_kinds_mask,
    UmiWorkbenchContextLinkMode default_mode)
{
    UmiWorkbenchContextLinkGroupProfile profile;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || group_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_link_group_profile_init(&profile, group_id);
    status = umi_workbench_context_link_group_profile_set_primary(&profile, title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    profile.colour = colour;
    profile.mode = default_mode;
    profile.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED;
    profile.flags = allowed_kinds_mask == 0U
        ? UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK
        : allowed_kinds_mask;
    status = umi_workbench_context_link_group_catalogue_upsert(&service->groups, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the workbench context link service bind panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_service_bind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *binding_id,
    const char *panel_id,
    const char *application_id,
    const char *group_id,
    UmiContextKind context_kind,
    UmiWorkbenchContextLinkMode mode)
{
    UmiWorkbenchContextLinkPanelBinding binding;
    const UmiWorkbenchContextLinkGroupProfile *group;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || binding_id == NULL || panel_id == NULL ||
        application_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_kind_allowed(group->flags, context_kind) &&
        context_kind != UMI_CONTEXT_KIND_GENERIC) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_link_panel_binding_init(&binding, binding_id);
    status = umi_workbench_context_link_panel_binding_set_primary(&binding, panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_panel_binding_set_secondary(
        &binding, application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_copy_text(
        binding.group_id, sizeof(binding.group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    binding.context_kind = context_kind;
    binding.colour = group->colour;
    binding.mode = mode;
    binding.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED;
    status = umi_workbench_context_link_binding_registry_upsert(
        &service->bindings, &binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the workbench context link service unbind panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_unbind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *panel_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_binding_registry_remove_panel(
        &service->bindings, panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the workbench context link service publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_service_publish(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkGroupProfile *group;
    size_t index;
    bool source_authorised = source_panel_id == NULL || source_panel_id[0] == '\0';
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || group_id == NULL || payload == NULL ||
        out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (service->suspended) return UMI_STATUS_BUSY;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_link_kind_allowed(group->flags, payload->kind)) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!source_authorised) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < service->bindings.count; ++index) {
            const UmiWorkbenchContextLinkPanelBinding *binding = &service->bindings.items[index];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(binding->group_id, group_id) == 0 &&
                strcmp(binding->panel_id, source_panel_id) == 0 &&
                mode_can_publish(binding->mode)) {
                source_authorised = true;
                break;
            }
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!source_authorised) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    ++service->sequence;
    status = umi_workbench_context_link_active_context_store_put(
        &service->active, group_id, payload, service->sequence, now_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_history_store_append(
        &service->history, group_id, payload, service->sequence, now_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = build_deliveries(service, group_id, source_panel_id,
                              payload->kind, group->colour, out_deliveries);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    ++service->metrics.publish_count;
    ++service->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link service current operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_service_current(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return NULL;
    return umi_workbench_context_link_active_context_store_find(
        &service->active, group_id);
}

/*
 * Provide the workbench context link service pin current operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_service_pin_current(
    UmiWorkbenchContextLinkService *service,
    const char *pin_id,
    const char *group_id,
    uint64_t now_ms)
{
    const UmiWorkbenchContextLinkActiveSlot *slot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || pin_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    slot = umi_workbench_context_link_service_current(service, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slot == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_workbench_context_link_pin_store_put(
        &service->pins, pin_id, group_id, &slot->payload, now_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        ++service->metrics.pin_count;
        ++service->revision;
    }
    return status;
}

/* Provide the restore item operation used by this module and its client applications. */
static UmiStatus restore_item(
    UmiWorkbenchContextLinkService *service,
    const UmiWorkbenchContextLinkHistoryItem *item,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkGroupProfile *group;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, item->group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group == NULL) return UMI_STATUS_NOT_FOUND;
    ++service->sequence;
    status = umi_workbench_context_link_active_context_store_put(
        &service->active, item->group_id, &item->payload,
        service->sequence, item->published_at_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = build_deliveries(service, item->group_id, NULL,
                              item->payload.kind, group->colour, out_deliveries);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        ++service->metrics.navigation_count;
        ++service->revision;
    }
    return status;
}

/*
 * Provide the workbench context link service restore previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_service_restore_previous(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkHistoryItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || group_id == NULL || out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_workbench_context_link_history_store_previous(
        &service->history, group_id);
    return restore_item(service, item, out_deliveries);
}

/*
 * Provide the workbench context link service restore next operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_service_restore_next(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkHistoryItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || group_id == NULL || out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_workbench_context_link_history_store_next(
        &service->history, group_id);
    return restore_item(service, item, out_deliveries);
}

/*
 * Provide the workbench context link service set suspended operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_service_set_suspended(
    UmiWorkbenchContextLinkService *service,
    bool suspended)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}

/*
 * Provide the workbench context link service revision operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_service_revision(
    const UmiWorkbenchContextLinkService *service)
{
    return service == NULL ? 0U : service->revision;
}
