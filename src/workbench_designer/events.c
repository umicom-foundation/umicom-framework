/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/events.c
 *
 * PURPOSE:
 *   Implement synchronous ordered event publication for controller and
 *   application composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/events.h"
#include "internal.h"


/*
 * Initialise workbench designer event bus from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_event_bus_init(UmiWorkbenchDesignerEventBus *bus)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL) return;
    (void)memset(bus, 0, sizeof(*bus));
    bus->next_sequence = 1U;
}

/*
 * Provide the workbench designer event subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_event_subscribe(
    UmiWorkbenchDesignerEventBus *bus,
    const char *subscription_id,
    UmiWorkbenchDesignerEventListener listener,
    void *context)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL || subscription_id == NULL || subscription_id[0] == '\0' ||
        listener == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->subscriptions[index].subscription_id, subscription_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the workbench designer event unsubscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_event_unsubscribe(
    UmiWorkbenchDesignerEventBus *bus,
    const char *subscription_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL || subscription_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->subscriptions[index].subscription_id, subscription_id) == 0) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
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

/*
 * Provide the workbench designer event publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_event_publish(
    UmiWorkbenchDesignerEventBus *bus,
    UmiWorkbenchDesignerEvent *event)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL || event == NULL || event->event_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    event->sequence = bus->next_sequence++;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (bus->subscriptions[index].enabled) {
            bus->subscriptions[index].listener(
                event, bus->subscriptions[index].context);
        }
    }
    bus->published_count += 1U;
    bus->revision += 1U;
    return UMI_STATUS_OK;
}
