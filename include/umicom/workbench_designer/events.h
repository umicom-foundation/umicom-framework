/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/events.h
 *
 * PURPOSE:
 *   Publish immutable designer lifecycle and state-change events to Framework
 *   consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_EVENTS_H
#define UMICOM_WORKBENCH_DESIGNER_EVENTS_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerEvent {
    UmiWorkbenchDesignerEventKind kind;
    char event_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t timestamp_ms;
    uint64_t sequence;
} UmiWorkbenchDesignerEvent;

typedef void (*UmiWorkbenchDesignerEventListener)(const UmiWorkbenchDesignerEvent *event, void *context);

typedef struct UmiWorkbenchDesignerEventSubscription {
    char subscription_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerEventListener listener;
    void *context;
    bool enabled;
} UmiWorkbenchDesignerEventSubscription;

typedef struct UmiWorkbenchDesignerEventBus {
    UmiWorkbenchDesignerEventSubscription subscriptions[UMI_WORKBENCH_DESIGNER_MAX_LISTENERS];
    size_t count;
    uint64_t next_sequence;
    uint64_t published_count;
    uint64_t revision;
} UmiWorkbenchDesignerEventBus;

void umi_workbench_designer_event_bus_init(UmiWorkbenchDesignerEventBus *bus);
UmiStatus umi_workbench_designer_event_subscribe(UmiWorkbenchDesignerEventBus *bus, const char *subscription_id, UmiWorkbenchDesignerEventListener listener, void *context);
UmiStatus umi_workbench_designer_event_unsubscribe(UmiWorkbenchDesignerEventBus *bus, const char *subscription_id);
UmiStatus umi_workbench_designer_event_publish(UmiWorkbenchDesignerEventBus *bus, UmiWorkbenchDesignerEvent *event);

#ifdef __cplusplus
}
#endif

#endif
