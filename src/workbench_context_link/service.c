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

static bool mode_can_follow(UmiWorkbenchContextLinkMode mode)
{
    return mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW ||
           mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
}

static bool mode_can_publish(UmiWorkbenchContextLinkMode mode)
{
    return mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_PUBLISH ||
           mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
}

static UmiStatus build_deliveries(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    const char *source_panel_id,
    UmiContextKind kind,
    UmiContextChannelColour colour,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    size_t index;
    if (out_deliveries == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_deliveries, 0, sizeof(*out_deliveries));
    out_deliveries->sequence = service->sequence;
    for (index = 0U; index < service->bindings.count; ++index) {
        const UmiWorkbenchContextLinkPanelBinding *binding = &service->bindings.items[index];
        UmiWorkbenchContextLinkDelivery *delivery;
        if (strcmp(binding->group_id, group_id) != 0) continue;
        if (!mode_can_follow(binding->mode)) continue;
        if (binding->state == UMI_WORKBENCH_CONTEXT_LINK_STATE_SUSPENDED) continue;
        if (binding->context_kind != kind && binding->context_kind != UMI_CONTEXT_KIND_GENERIC) continue;
        if (source_panel_id != NULL && source_panel_id[0] != '\0' &&
            strcmp(binding->panel_id, source_panel_id) == 0) continue;
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

void umi_workbench_context_link_service_init(
    UmiWorkbenchContextLinkService *service)
{
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

void umi_workbench_context_link_service_destroy(
    UmiWorkbenchContextLinkService *service)
{
    if (service == NULL) return;
    umi_workbench_context_link_group_catalogue_destroy(&service->groups);
    umi_workbench_context_link_binding_registry_destroy(&service->bindings);
    umi_workbench_context_link_active_context_store_destroy(&service->active);
    umi_workbench_context_link_history_store_destroy(&service->history);
    umi_workbench_context_link_pin_store_destroy(&service->pins);
    memset(service, 0, sizeof(*service));
}

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
    if (service == NULL || group_id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_link_group_profile_init(&profile, group_id);
    status = umi_workbench_context_link_group_profile_set_primary(&profile, title);
    if (status != UMI_STATUS_OK) return status;
    profile.colour = colour;
    profile.mode = default_mode;
    profile.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED;
    profile.flags = allowed_kinds_mask == 0U
        ? UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK
        : allowed_kinds_mask;
    status = umi_workbench_context_link_group_catalogue_upsert(&service->groups, &profile);
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

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
    if (service == NULL || binding_id == NULL || panel_id == NULL ||
        application_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, group_id);
    if (group == NULL) return UMI_STATUS_NOT_FOUND;
    if (!umi_workbench_context_link_kind_allowed(group->flags, context_kind) &&
        context_kind != UMI_CONTEXT_KIND_GENERIC) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_link_panel_binding_init(&binding, binding_id);
    status = umi_workbench_context_link_panel_binding_set_primary(&binding, panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_panel_binding_set_secondary(
        &binding, application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_copy_text(
        binding.group_id, sizeof(binding.group_id), group_id);
    if (status != UMI_STATUS_OK) return status;
    binding.context_kind = context_kind;
    binding.colour = group->colour;
    binding.mode = mode;
    binding.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ATTACHED;
    status = umi_workbench_context_link_binding_registry_upsert(
        &service->bindings, &binding);
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

UmiStatus umi_workbench_context_link_service_unbind_panel(
    UmiWorkbenchContextLinkService *service,
    const char *panel_id)
{
    UmiStatus status;
    if (service == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_binding_registry_remove_panel(
        &service->bindings, panel_id);
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

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
    if (service == NULL || group_id == NULL || payload == NULL ||
        out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (service->suspended) return UMI_STATUS_BUSY;
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, group_id);
    if (group == NULL) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_NOT_FOUND;
    }
    if (!umi_workbench_context_link_kind_allowed(group->flags, payload->kind)) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!source_authorised) {
        for (index = 0U; index < service->bindings.count; ++index) {
            const UmiWorkbenchContextLinkPanelBinding *binding = &service->bindings.items[index];
            if (strcmp(binding->group_id, group_id) == 0 &&
                strcmp(binding->panel_id, source_panel_id) == 0 &&
                mode_can_publish(binding->mode)) {
                source_authorised = true;
                break;
            }
        }
    }
    if (!source_authorised) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    ++service->sequence;
    status = umi_workbench_context_link_active_context_store_put(
        &service->active, group_id, payload, service->sequence, now_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_history_store_append(
        &service->history, group_id, payload, service->sequence, now_ms);
    if (status != UMI_STATUS_OK) return status;
    status = build_deliveries(service, group_id, source_panel_id,
                              payload->kind, group->colour, out_deliveries);
    if (status != UMI_STATUS_OK) return status;
    ++service->metrics.publish_count;
    ++service->revision;
    return UMI_STATUS_OK;
}

const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_service_current(
    const UmiWorkbenchContextLinkService *service,
    const char *group_id)
{
    if (service == NULL) return NULL;
    return umi_workbench_context_link_active_context_store_find(
        &service->active, group_id);
}

UmiStatus umi_workbench_context_link_service_pin_current(
    UmiWorkbenchContextLinkService *service,
    const char *pin_id,
    const char *group_id,
    uint64_t now_ms)
{
    const UmiWorkbenchContextLinkActiveSlot *slot;
    UmiStatus status;
    if (service == NULL || pin_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    slot = umi_workbench_context_link_service_current(service, group_id);
    if (slot == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_workbench_context_link_pin_store_put(
        &service->pins, pin_id, group_id, &slot->payload, now_ms);
    if (status == UMI_STATUS_OK) {
        ++service->metrics.pin_count;
        ++service->revision;
    }
    return status;
}

static UmiStatus restore_item(
    UmiWorkbenchContextLinkService *service,
    const UmiWorkbenchContextLinkHistoryItem *item,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkGroupProfile *group;
    UmiStatus status;
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    group = umi_workbench_context_link_group_catalogue_find_const(
        &service->groups, item->group_id);
    if (group == NULL) return UMI_STATUS_NOT_FOUND;
    ++service->sequence;
    status = umi_workbench_context_link_active_context_store_put(
        &service->active, item->group_id, &item->payload,
        service->sequence, item->published_at_ms);
    if (status != UMI_STATUS_OK) return status;
    status = build_deliveries(service, item->group_id, NULL,
                              item->payload.kind, group->colour, out_deliveries);
    if (status == UMI_STATUS_OK) {
        ++service->metrics.navigation_count;
        ++service->revision;
    }
    return status;
}

UmiStatus umi_workbench_context_link_service_restore_previous(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkHistoryItem *item;
    if (service == NULL || group_id == NULL || out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_workbench_context_link_history_store_previous(
        &service->history, group_id);
    return restore_item(service, item, out_deliveries);
}

UmiStatus umi_workbench_context_link_service_restore_next(
    UmiWorkbenchContextLinkService *service,
    const char *group_id,
    UmiWorkbenchContextLinkDeliveryBatch *out_deliveries)
{
    const UmiWorkbenchContextLinkHistoryItem *item;
    if (service == NULL || group_id == NULL || out_deliveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_workbench_context_link_history_store_next(
        &service->history, group_id);
    return restore_item(service, item, out_deliveries);
}

void umi_workbench_context_link_service_set_suspended(
    UmiWorkbenchContextLinkService *service,
    bool suspended)
{
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}

uint64_t umi_workbench_context_link_service_revision(
    const UmiWorkbenchContextLinkService *service)
{
    return service == NULL ? 0U : service->revision;
}
