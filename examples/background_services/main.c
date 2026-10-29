/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/background_services/main.c
 *
 * PURPOSE:
 *   Compose installed watcher, task and process services with checked ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/watcher.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/process_supervisor.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ExampleServices {
    UmiWatcher *watcher;
    UmiTaskQueue *queue;
    UmiProcessSupervisor *supervisor;
    UmiTask *task; /* The caller's reference, separate from queue retention. */
    atomic_uint delivered;
    atomic_int callback_status;
} ExampleServices;

static UmiStatus ReportStatus(const char *operation, UmiStatus status)
{
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "%s: %s\n", operation, umi_status_text(status));
    return status;
}

static void OnFileEvent(const UmiWatchEvent *event, void *data)
{
    ExampleServices *services = data;
    UmiWatcher *borrowed = services->watcher;
    /* A sink cannot release the object that is delivering its event, even
     * when the fake backend calls the sink synchronously on a task worker. */
    UmiStatus status = UmiWatcherRelease(&borrowed);
    if (event == NULL || status != UMI_STATUS_INVALID_STATE ||
        borrowed != services->watcher) {
        atomic_store(&services->callback_status, (int)UMI_STATUS_INVALID_STATE);
        return;
    }
    status = UmiWatcherRequestStop(services->watcher);
    if (status != UMI_STATUS_OK)
        atomic_store(&services->callback_status, (int)status);
    atomic_fetch_add(&services->delivered, 1U);
}

static UmiStatus DeliverEvent(UmiTaskContext *context, void *data)
{
    ExampleServices *services = data;
    (void)context;
    if (!UmiTaskQueueIsCurrentWorker(services->queue))
        return UMI_STATUS_INVALID_STATE;
    return umi_watcher_fake_emit(services->watcher, UMI_WATCH_MODIFIED,
        "example-source.c", NULL, 0);
}

/* The example's controller is the only external user during this operation.
 * Keep the entire heap graph if any step fails; returning a void cleanup
 * result would not grant permission to free callback data. */
static UmiStatus CloseServices(ExampleServices **inOutServices)
{
    ExampleServices *services = *inOutServices;
    UmiStatus status;
    if (services == NULL) return UMI_STATUS_OK;
    if (services->watcher != NULL) {
        status = umi_watcher_stop(services->watcher);
        if (status != UMI_STATUS_OK) return ReportStatus("Stop watcher", status);
    }
    if (services->supervisor != NULL) {
        status = umi_process_supervisor_shutdown(services->supervisor);
        if (status != UMI_STATUS_OK) return ReportStatus("Stop supervisor", status);
    }
    if (services->queue != NULL) {
        status = umi_task_queue_shutdown(services->queue, 1);
        if (status != UMI_STATUS_OK) return ReportStatus("Stop queue", status);
    }
    status = UmiWatcherRelease(&services->watcher);
    if (status != UMI_STATUS_OK) return ReportStatus("Release watcher", status);
    status = UmiProcessSupervisorRelease(&services->supervisor);
    if (status != UMI_STATUS_OK) return ReportStatus("Release supervisor", status);
    status = UmiTaskQueueReleaseStopped(&services->queue);
    if (status != UMI_STATUS_OK) return ReportStatus("Release queue", status);
    umi_task_destroy(services->task);
    free(services);
    *inOutServices = NULL;
    return UMI_STATUS_OK;
}

static UmiStatus CreateServices(ExampleServices *services)
{
    UmiTaskQueueConfig queue_config = {1U, 2U};
    UmiStatus status = umi_task_queue_create(&queue_config, &services->queue);
    if (status != UMI_STATUS_OK) return ReportStatus("Create queue", status);
    UmiWatcherConfig watcher_config = umi_watcher_config_default(".");
    watcher_config.backend = UMI_WATCHER_FAKE;
    watcher_config.sink = OnFileEvent;
    watcher_config.sink_user_data = services;
    status = umi_watcher_create(&watcher_config, &services->watcher);
    if (status != UMI_STATUS_OK) return ReportStatus("Create watcher", status);
    status = umi_watcher_start(services->watcher);
    if (status != UMI_STATUS_OK) return ReportStatus("Start watcher", status);
    status = umi_process_supervisor_create(NULL, &services->supervisor);
    if (status != UMI_STATUS_OK) return ReportStatus("Create supervisor", status);
    UmiTaskConfig task_config = {
        "Deliver example file event", DeliverEvent, services, NULL, NULL
    };
    return ReportStatus("Create task", umi_task_create(&task_config, &services->task));
}

static UmiStatus RunServices(ExampleServices *services, const char *executable)
{
    const char *arguments[] = {"--child"};
    UmiProcessRequest request = {0};
    UmiProcessJobId id = 0U;
    UmiProcessJobSnapshot snapshot;
    request.program = executable;
    request.arguments = arguments;
    request.argument_count = 1U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = 5000U;
    UmiStatus status = umi_task_queue_submit(services->queue, services->task);
    if (status != UMI_STATUS_OK) return ReportStatus("Submit task", status);
    status = umi_process_supervisor_submit(services->supervisor,
        "Installed SDK example child", &request, &id);
    if (status != UMI_STATUS_OK) return ReportStatus("Submit child", status);
    status = umi_task_wait(services->task, 5000U);
    if (status != UMI_STATUS_OK) return ReportStatus("Wait for task", status);
    status = umi_task_result(services->task);
    if (status != UMI_STATUS_OK) return ReportStatus("Task result", status);
    status = (UmiStatus)atomic_load(&services->callback_status);
    if (status != UMI_STATUS_OK) return ReportStatus("Watcher callback", status);
    if (atomic_load(&services->delivered) != 1U)
        return ReportStatus("Expected one event", UMI_STATUS_INVALID_STATE);
    status = umi_process_supervisor_wait(services->supervisor, id, 10000U);
    if (status != UMI_STATUS_OK) return ReportStatus("Wait for child", status);
    status = umi_process_supervisor_snapshot(services->supervisor, id, &snapshot);
    if (status != UMI_STATUS_OK) return ReportStatus("Read child result", status);
    if (snapshot.state != UMI_PROCESS_JOB_SUCCEEDED || snapshot.exit_code != 0 ||
        strstr(snapshot.output, "umicom-background-example-child") == NULL)
        return ReportStatus("Unexpected child result", UMI_STATUS_INVALID_STATE);
    /* Reading the result acknowledges this job. Its slot can now be released
     * without closing the supervisor or losing any other result. */
    return ReportStatus("Release completed job",
        UmiProcessSupervisorReleaseJob(services->supervisor, id));
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--child") == 0) {
        (void)puts("umicom-background-example-child");
        return 0;
    }
    if (argc != 3 || strcmp(argv[1], "--verify") != 0 || argv[2][0] == '\0') {
        (void)fprintf(stderr, "Usage: %s --verify <absolute-path-to-this-executable>\n",
            argc > 0 ? argv[0] : "umicom-background-services-example");
        return 2;
    }
    ExampleServices *services = calloc(1U, sizeof(*services));
    if (services == NULL) return 1;
    atomic_init(&services->delivered, 0U);
    atomic_init(&services->callback_status, (int)UMI_STATUS_OK);
    UmiStatus status = CreateServices(services);
    if (status == UMI_STATUS_OK) status = RunServices(services, argv[2]);
    UmiStatus close_status = CloseServices(&services);
    if (close_status != UMI_STATUS_OK) {
        /* No stack payload is left for retained workers to access. The failed
         * process exits with the graph retained; it never advertises a pass. */
        (void)fprintf(stderr, "Background owner retained after incomplete shutdown.\n");
        return 1;
    }
    if (status != UMI_STATUS_OK) return 1;
    (void)puts("Installed watcher, queue and process lifecycle completed.");
    return 0;
}
