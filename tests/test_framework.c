/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_framework.c
 *
 * PURPOSE:
 *   Exercise the integrated Framework foundation, including Master Controller
 *   lifecycle, messages, scheduling, journal replay, and optional SQLite data
 *   persistence through public C23 contracts.
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

/*
 * Exercise on start and return a clear result when the behaviour no longer matches its
 * contract.
 */
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

/*
 * Exercise on stop and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus on_stop(UmiModuleContext *context)
{
    (void)context;
    stops++;
    return UMI_STATUS_OK;
}

/*
 * Exercise echo command and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus echo_command(const char *payload, void *user_data)
{
    (void)user_data;
    return strcmp(payload, "hello") == 0 ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Exercise state query and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus state_query(const char *request, char *response, size_t capacity, void *user_data)
{
    (void)request;
    (void)user_data;
    (void)snprintf(response, capacity, "ready");
    return UMI_STATUS_OK;
}

/*
 * Exercise scheduled task and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void scheduled_task(uint64_t task_id, void *user_data)
{
    (void)task_id;
    (void)user_data;
    scheduled_runs++;
}

/*
 * Exercise replay handler and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void replay_handler(const UmiMessageEnvelope *event, void *user_data)
{
    (void)user_data;
    /* Apply this branch only when its contract condition is satisfied. */
    if (event != 0) replay_events += 1U;
}

/*
 * Exercise test clock scheduler and return a clear result when the behaviour no longer
 * matches its contract.
 */
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

/*
 * Exercise test journal and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void test_journal(void)
{
    const char *path = "umicom-framework-test.journal";
    UmiJournal *journal = 0;
    UmiEventBus *bus = 0;
    UmiMessageEnvelope event = {
        .structure_size = (uint32_t)sizeof(UmiMessageEnvelope),
        .schema_version = 1U,
        .kind = UMI_MESSAGE_EVENT,
        .sequence = 7U,
        .correlation_id = 88U,
        .name = "umi.test.event.v1",
        .payload = "payload"
    };
    size_t count = 0U;
    size_t incremental_count = 0U;
    uint64_t last_sequence = 0U;
    (void)remove(path);
    assert(umi_journal_open(path, &journal) == UMI_STATUS_OK);
    assert(umi_journal_append(journal, &event) == UMI_STATUS_OK);
    umi_journal_close(journal);
    assert(umi_journal_last_sequence(path, &last_sequence) == UMI_STATUS_OK);
    assert(last_sequence == 7U);
    assert(umi_event_bus_create(&bus) == UMI_STATUS_OK);
    assert(umi_event_bus_subscribe(bus, "umi.test.event.v1", replay_handler, 0) == UMI_STATUS_OK);
    assert(umi_journal_replay(path, bus, &count) == UMI_STATUS_OK);
    assert(count == 1U);
    assert(replay_events == 1U);
    assert(umi_journal_replay_after(path,
                                    bus,
                                    7U,
                                    &incremental_count,
                                    &last_sequence) == UMI_STATUS_OK);
    assert(incremental_count == 0U);
    assert(last_sequence == 7U);
    umi_event_bus_destroy(bus);
    (void)remove(path);
}

/*
 * Exercise test sqlite data server and return a clear result when the behaviour no longer
 * matches its contract.
 */
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiMasterController *master = 0;
    UmiMasterControllerConfig config = {"Framework Test", 0, 0};
    UmiModuleDescriptor module = {
        .structure_size = sizeof(UmiModuleDescriptor),
        .abi_version = UMICOM_FRAMEWORK_ABI_VERSION,
        .module_id = "test.module",
        .display_name = "Test Module",
        .module_version = {0U, 5U, 0U},
        .kind = UMI_MODULE_SERVICE,
        .provided_capabilities = NULL,
        .required_capabilities = NULL,
        .optional_capabilities = NULL,
        .requested_permissions = NULL,
        .module_state = NULL,
        .lifecycle = {
            .configure = NULL,
            .initialise = NULL,
            .start = on_start,
            .quiesce = NULL,
            .stop = on_stop,
            .destroy = NULL
        }
    };
    char value[64];
    char reply[64];
    UmiDocument doc;

    assert(UMICOM_FRAMEWORK_VERSION_MINOR == 9);
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
