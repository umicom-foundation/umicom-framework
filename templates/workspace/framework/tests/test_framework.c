/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/tests/test_framework.c
 *
 * PURPOSE:
 *   Implement the test framework behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static int starts = 0;
static int stops = 0;
static int scheduled_runs = 0;
static size_t replay_events = 0U;

static UmiStatus on_start(UmiModuleContext *context)
{
    starts++;
    assert(context->events != 0);
    assert(context->commands != 0);
    assert(context->queries != 0);
    assert(context->data_server != 0);
    assert(context->clock != 0);
    assert(context->scheduler != 0);
    return UMI_STATUS_OK;
}

static UmiStatus on_stop(UmiModuleContext *context)
{
    (void)context;
    stops++;
    return UMI_STATUS_OK;
}

static UmiStatus echo_command(const char *payload, void *user_data)
{
    (void)user_data;
    return strcmp(payload, "hello") == 0 ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus state_query(const char *request, char *response, size_t capacity, void *user_data)
{
    (void)request;
    (void)user_data;
    (void)snprintf(response, capacity, "ready");
    return UMI_STATUS_OK;
}

static void scheduled_task(uint64_t task_id, void *user_data)
{
    (void)task_id;
    (void)user_data;
    scheduled_runs++;
}

static void replay_handler(const UmiMessageEnvelope *event, void *user_data)
{
    (void)user_data;
    if (event != 0) replay_events += 1U;
}

static void test_clock_scheduler(void)
{
    UmiClock clock = umi_clock_fake(1000U);
    UmiScheduler *scheduler = 0;
    uint64_t task_id = 0U;
    assert(clock.state != 0);
    assert(umi_scheduler_create(&clock, &scheduler) == UMI_STATUS_OK);
    assert(umi_scheduler_schedule(scheduler, 2000U, scheduled_task, 0, &task_id) == UMI_STATUS_OK);
    assert(task_id != 0U);
    assert(umi_scheduler_run_due(scheduler) == 0U);
    assert(umi_clock_fake_advance(&clock, 1000U) == UMI_STATUS_OK);
    assert(umi_scheduler_run_due(scheduler) == 1U);
    assert(scheduled_runs == 1);
    assert(umi_scheduler_pending(scheduler) == 0U);
    umi_scheduler_destroy(scheduler);
    umi_clock_dispose(&clock);
}

static void test_journal(void)
{
    const char *path = "umicom-framework-test.journal";
    UmiJournal *journal = 0;
    UmiEventBus *bus = 0;
    UmiMessageEnvelope event = {
        sizeof(UmiMessageEnvelope), 1U, UMI_MESSAGE_EVENT,
        7U, 88U, "umi.test.event.v1", "payload"
    };
    size_t count = 0U;
    (void)remove(path);
    assert(umi_journal_open(path, &journal) == UMI_STATUS_OK);
    assert(umi_journal_append(journal, &event) == UMI_STATUS_OK);
    umi_journal_close(journal);
    assert(umi_event_bus_create(&bus) == UMI_STATUS_OK);
    assert(umi_event_bus_subscribe(bus, "umi.test.event.v1", replay_handler, 0) == UMI_STATUS_OK);
    assert(umi_journal_replay(path, bus, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(replay_events == 1U);
    umi_event_bus_destroy(bus);
    (void)remove(path);
}

static void test_sqlite_data_server(void)
{
#ifdef UMICOM_HAS_SQLITE
    const char *path = "umicom-framework-test.sqlite";
    UmiDataServer *server = 0;
    char value[64];
    (void)remove(path);
    assert(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    assert(strcmp(umi_data_server_backend_name(server), "sqlite") == 0);
    assert(umi_data_server_set(server, "alpha", "one") == UMI_STATUS_OK);
    assert(umi_data_server_get(server, "alpha", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "one") == 0);
    assert(umi_data_server_count(server) == 1U);
    assert(umi_data_server_delete(server, "alpha") == UMI_STATUS_OK);
    umi_data_server_destroy(server);
    (void)remove(path);
#endif
}

int main(void)
{
    UmiMasterController *master = 0;
    UmiMasterControllerConfig config = {"Framework Test", 0, 0};
    UmiModuleDescriptor module = {
        sizeof(UmiModuleDescriptor), UMICOM_FRAMEWORK_ABI_VERSION,
        "test.module", "Test Module", {0, 4, 0}, UMI_MODULE_SERVICE,
        0, 0, 0, {0, 0, on_start, 0, on_stop, 0}
    };
    char value[64];
    char reply[64];
    UmiDocument doc;

    assert(UMICOM_FRAMEWORK_VERSION_MINOR == 4);
    assert(umi_master_controller_create(&config, &master) == UMI_STATUS_OK);
    assert(umi_master_controller_register(master, &module) == UMI_STATUS_OK);
    assert(umi_command_bus_register(umi_master_controller_commands(master), "test.echo", echo_command, 0) == UMI_STATUS_OK);
    assert(umi_query_bus_register(umi_master_controller_queries(master), "test.state", state_query, 0) == UMI_STATUS_OK);
    assert(umi_master_controller_start(master) == UMI_STATUS_OK);
    assert(starts == 1);
    assert(umi_command_bus_execute(umi_master_controller_commands(master), "test.echo", "hello") == UMI_STATUS_OK);
    assert(umi_query_bus_execute(umi_master_controller_queries(master), "test.state", "", reply, sizeof(reply)) == UMI_STATUS_OK);
    assert(strcmp(reply, "ready") == 0);
    assert(umi_data_server_set(umi_master_controller_data_server(master), "project.name", "Umicom") == UMI_STATUS_OK);
    assert(umi_data_server_get(umi_master_controller_data_server(master), "project.name", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "Umicom") == 0);
    umi_document_init(&doc);
    assert(umi_document_set_text(&doc, "abc") == UMI_STATUS_OK);
    assert(doc.dirty == 1 && doc.revision == 1U);
    umi_document_dispose(&doc);
    assert(umi_master_controller_stop(master) == UMI_STATUS_OK);
    assert(stops == 1);
    umi_master_controller_destroy(master);

    test_clock_scheduler();
    test_journal();
    test_sqlite_data_server();
    return 0;
}
