/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_events_commands.c
 *
 * PURPOSE:
 *   Verify typed command validation and event fan-out without coupling layout
 *   state changes to a graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

typedef struct EventCapture {
    size_t count;
    UmiWorkbenchLayoutEvent last;
} EventCapture;

static void capture_event(
    void *context,
    const UmiWorkbenchLayoutEvent *event)
{
    EventCapture *capture = (EventCapture *)context;
    capture->count += 1U;
    capture->last = *event;
}

int main(void)
{
    UmiWorkbenchLayoutCommand command;
    UmiWorkbenchLayoutCommandResult command_result;
    UmiWorkbenchLayoutEventBus bus;
    UmiWorkbenchLayoutEvent event;
    EventCapture capture;
    UmiWorkbenchLayoutEventSubscription subscription;

    umi_workbench_layout_command_init(
        &command,
        UMI_WORKBENCH_LAYOUT_COMMAND_CREATE,
        "command.create",
        "developer",
        "correlation.command");
    test_copy_text(
        command.layout_id,
        sizeof(command.layout_id),
        "layout.command");
    test_copy_text(
        command.name,
        sizeof(command.name),
        "Command Layout");
    TEST_STATUS_OK(umi_workbench_layout_command_validate(&command));
    TEST_REQUIRE(
        umi_workbench_layout_command_mutates_state(&command),
        "Create command must be classified as a mutation");
    TEST_REQUIRE(
        !umi_workbench_layout_command_requires_layout(&command),
        "Create command creates rather than requires a loaded layout");
    umi_workbench_layout_command_result_init(&command_result, &command);
    TEST_REQUIRE(
        strcmp(command_result.command_id, "command.create") == 0,
        "Command result must retain correlation identity");

    umi_workbench_layout_event_bus_init(&bus);
    (void)memset(&capture, 0, sizeof(capture));
    (void)memset(&subscription, 0, sizeof(subscription));
    subscription.structure_size = sizeof(subscription);
    test_copy_text(
        subscription.subscription_id,
        sizeof(subscription.subscription_id),
        "subscription.command-updates");
    subscription.event_mask = umi_workbench_layout_event_mask(
        UMI_WORKBENCH_LAYOUT_EVENT_UPDATED);
    subscription.listener = capture_event;
    subscription.listener_context = &capture;
    subscription.active = true;
    TEST_STATUS_OK(umi_workbench_layout_event_bus_subscribe(
        &bus, &subscription));
    umi_workbench_layout_event_init(
        &event,
        UMI_WORKBENCH_LAYOUT_EVENT_UPDATED,
        "event.updated",
        "layout.command");
    event.layout_revision = 2U;
    TEST_STATUS_OK(umi_workbench_layout_event_bus_publish(&bus, &event));
    TEST_REQUIRE(capture.count == 1U, "Matching subscriber must receive event");
    TEST_REQUIRE(
        capture.last.layout_revision == 2U,
        "Subscriber must receive immutable event evidence");
    TEST_STATUS_OK(umi_workbench_layout_event_bus_unsubscribe(
        &bus, "subscription.command-updates"));
    TEST_STATUS_OK(umi_workbench_layout_event_bus_publish(&bus, &event));
    TEST_REQUIRE(capture.count == 1U, "Removed subscriber must not receive events");
    return 0;
}
