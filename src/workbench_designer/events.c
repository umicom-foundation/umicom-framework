/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/events.c
 *
 * PURPOSE:
 *   Implement synchronous ordered event publication for controller and
 *   application composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/events.h"
#include "internal.h"


void umi_workbench_designer_event_bus_init(UmiWorkbenchDesignerEventBus *bus)
{
    if (bus == NULL) return;
    (void)memset(bus, 0, sizeof(*bus));
    bus->next_sequence = 1U;
}

UmiStatus umi_workbench_designer_event_subscribe(
    UmiWorkbenchDesignerEventBus *bus,
    const char *subscription_id,
    UmiWorkbenchDesignerEventListener listener,
    void *context)
{
    size_t index;
    if (bus == NULL || subscription_id == NULL || subscription_id[0] == '\0' ||
        listener == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < bus->count; ++index) {
        if (strcmp(bus->subscriptions[index].subscription_id, subscription_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (bus->count >= UMI_WORKBENCH_DESIGNER_MAX_LISTENERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_workbench_designer_copy_text(
        bus->subscriptions[bus->count].subscription_id,
        sizeof(bus->subscriptions[bus->count].subscription_id),
        subscription_id);
    bus->subscriptions[bus->count].listener = listener;
    bus->subscriptions[bus->count].context = context;
    bus->subscriptions[bus->count].enabled = true;
    bus->count += 1U;
    bus->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_event_unsubscribe(
    UmiWorkbenchDesignerEventBus *bus,
    const char *subscription_id)
{
    size_t index;
    if (bus == NULL || subscription_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < bus->count; ++index) {
        if (strcmp(bus->subscriptions[index].subscription_id, subscription_id) == 0) {
            size_t move_index;
            for (move_index = index + 1U; move_index < bus->count; ++move_index) {
                bus->subscriptions[move_index - 1U] = bus->subscriptions[move_index];
            }
            bus->count -= 1U;
            bus->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_workbench_designer_event_publish(
    UmiWorkbenchDesignerEventBus *bus,
    UmiWorkbenchDesignerEvent *event)
{
    size_t index;
    if (bus == NULL || event == NULL || event->event_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    event->sequence = bus->next_sequence++;
    for (index = 0U; index < bus->count; ++index) {
        if (bus->subscriptions[index].enabled) {
            bus->subscriptions[index].listener(
                event, bus->subscriptions[index].context);
        }
    }
    bus->published_count += 1U;
    bus->revision += 1U;
    return UMI_STATUS_OK;
}
