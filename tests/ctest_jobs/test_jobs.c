/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ctest_jobs/test_jobs.c
 * Sammy Hegab | Umicom Foundation | MIT
 * Exercise real CTest processes, task ownership and copied discovery origins.
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_job.h"
#include "umicom/platform/threading.h"
#include "umicom/test_platform/ctest.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static UmiCtestJobRequest Request(const char *root, const char *name, const char *id)
{
    UmiCtestJobRequest request = {0};
    CHECK(strlen(root) < sizeof(request.build_directory));
    (void)snprintf(request.build_directory, sizeof(request.build_directory), "%s", root);
    (void)snprintf(request.test_id, sizeof(request.test_id), "%s", id);
    (void)snprintf(request.name, sizeof(request.name), "%s", name);
    memcpy(request.configuration, "Debug", sizeof("Debug"));
    request.enabled = 1;
    return request;
}
static UmiCtestJobSnapshot Snapshot(UmiCtestJob *job)
{
    UmiCtestJobSnapshot snapshot;
    CHECK(UmiCtestJobGetSnapshot(job, &snapshot) == UMI_STATUS_OK);
    return snapshot;
}
static UmiCtestJobSnapshot Wait(UmiCtestJob *job)
{
    for (unsigned attempt = 0U; attempt < 15000U; ++attempt) {
        UmiCtestJobSnapshot s = Snapshot(job);
        if (s.state >= UMI_TASK_SUCCEEDED) return s;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0); return Snapshot(job);
}
static UmiTestResult Result(UmiCtestJob *job, size_t index)
{
    UmiTestResult result;
    CHECK(UmiCtestJobResultAt(job, index, &result, NULL) == UMI_STATUS_OK);
    return result;
}
static int Exists(const char *root, const char *name)
{
    char path[1600]; (void)snprintf(path, sizeof(path), "%s/%s", root, name);
    FILE *file = fopen(path, "rb"); if (file == NULL) return 0;
    fclose(file); return 1;
}
static void RemoveMarker(const char *root, const char *name)
{
    char path[1600]; (void)snprintf(path, sizeof(path), "%s/%s", root, name);
    (void)remove(path);
}
static void AwaitMarker(const char *root)
{
    for (unsigned i = 0U; i < 10000U; ++i) {
        if (Exists(root, "ready")) return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}
static UmiStatus Block(UmiTaskContext *context, void *data)
{
    atomic_int *release = data;
    while (!atomic_load(release)) {
        if (umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return UMI_STATUS_OK;
}
static UmiStatus Count(UmiTaskContext *context, void *data)
{
    (void)context; atomic_fetch_add((atomic_int *)data, 1); return UMI_STATUS_OK;
}
typedef struct WrongThread { UmiCtestJob *job; UmiTaskQueue *queue; int passed; } WrongThread;
static int WrongOwner(void *data)
{
    WrongThread *test = data;
    test->passed = UmiCtestJobSubmit(test->job, test->queue) == UMI_STATUS_INVALID_STATE &&
        UmiCtestJobDestroy(test->job) == UMI_STATUS_INVALID_STATE;
    return 0;
}
static void Provenance(const char *mode, const char *root)
{
    UmiTestPlatformDiscoveryRegistry *registry = NULL;
    CHECK(umi_test_platform_discovery_registry_create(&registry) == UMI_STATUS_OK);
    UmiTestPlatformDiscoverySnapshot origin = {0};
    memcpy(origin.id, "discovery.notes.ctest", sizeof("discovery.notes.ctest"));
    memcpy(origin.provider, "ctest-json-v1", sizeof("ctest-json-v1"));
    (void)snprintf(origin.root_uri, sizeof(origin.root_uri), "%s", root);
    origin.state = 1;
    UmiTestPlatformItemSnapshot item = {0};
    memcpy(item.id, "notes.pass", sizeof("notes.pass"));
    memcpy(item.name, "notes.pass", sizeof("notes.pass"));
    memcpy(item.suite_id, "notes.ctest", sizeof("notes.ctest"));
    memcpy(item.kind, "test", sizeof("test"));
    memcpy(item.framework, "ctest", sizeof("ctest"));
    (void)snprintf(item.uri, sizeof(item.uri), "%s", root);
    memcpy(item.working_directory, "/unrelated test cwd", sizeof("/unrelated test cwd"));
    item.enabled = 1; item.discovered = 1;
    UmiCtestJobRequest request; memset(&request, 0x5a, sizeof(request));
    UmiCtestJobRequest before = request;
    if (strcmp(mode, "provenance-missing") != 0)
        CHECK(umi_test_platform_discovery_registry_upsert(registry, &origin) == UMI_STATUS_OK);
    if (strcmp(mode, "provenance-mismatch") == 0) strcpy(item.uri, "/wrong build root");
    if (strcmp(mode, "provenance-bounds") == 0) memset(item.suite_id, 'x', sizeof(item.suite_id));
    UmiStatus status = UmiTestPlatformCtestMakeRunRequest(&item, registry, &request);
    if (strcmp(mode, "provenance") == 0) {
        CHECK(status == UMI_STATUS_OK && strcmp(request.build_directory, root) == 0);
        CHECK(strcmp(request.configuration, "Debug") == 0 && request.enabled);
        CHECK(strcmp(request.name, "notes.pass") == 0);
    } else CHECK(status != UMI_STATUS_OK && memcmp(&before, &request, sizeof(before)) == 0);
    umi_test_platform_discovery_registry_destroy(registry);
}
int main(int argc, char **argv)
{
    CHECK(argc == 4);
    const char *mode = argv[1], *root = argv[2], *other = argv[3];
    if (strncmp(mode, "provenance", 10U) == 0) { Provenance(mode, root); return 0; }
    UmiCtestJobRequest requests[3] = {Request(root, "notes.pass", "one"),
        Request(root, "notes.pass", "two"), Request(root, "notes.pass", "three")};
    size_t count = 1U;
    UmiCtestJobOptions options = {0};
    UmiCtestJob *job = NULL;
    if (strcmp(mode, "invalid") == 0) {
        CHECK(UmiCtestJobCreate(NULL, 1U, NULL, &job) == UMI_STATUS_INVALID_ARGUMENT && job == NULL);
        CHECK(UmiCtestJobCreate(requests, 0U, NULL, &job) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobCreate(requests, 1U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobSubmit(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobDestroy(NULL) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(mode, "bounds") == 0) {
        options.repeat_count = UINT32_MAX;
        CHECK(UmiCtestJobCreate(requests, 1U, &options, &job) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiCtestJobCreate(requests, UMI_CTEST_JOB_MAX_ATTEMPTS + 1U, NULL, &job) == UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    if (strcmp(mode, "strings") == 0 || strcmp(mode, "duplicate") == 0 || strcmp(mode, "invalid-name") == 0) {
        if (strcmp(mode, "strings") == 0) memset(requests[0].configuration, 'x', sizeof(requests[0].configuration));
        if (strcmp(mode, "duplicate") == 0) { requests[1] = requests[0]; count = 2U; }
        if (strcmp(mode, "invalid-name") == 0) strcpy(requests[0].name, "bad\nname");
        CHECK(UmiCtestJobCreate(requests, count, NULL, &job) != UMI_STATUS_OK && job == NULL); return 0;
    }
    if (strcmp(mode, "literal") == 0) strcpy(requests[0].name, "notes.save[1]+");
    if (strcmp(mode, "repeat") == 0) { count = 3U; options.repeat_count = 2U; }
    if (strcmp(mode, "continue-failure") == 0 || strcmp(mode, "stop-failure") == 0) {
        count = 3U; strcpy(requests[1].name, "notes.fail");
        options.stop_on_failure = strcmp(mode, "stop-failure") == 0;
    }
    if (strcmp(mode, "disabled") == 0) { requests[0].enabled = 0; strcpy(requests[0].build_directory, "/does not exist"); }
    if (strcmp(mode, "skip") == 0) strcpy(requests[0].name, "notes.skip");
    if (strcmp(mode, "process-timeout") == 0) { strcpy(requests[0].name, "notes.wait"); requests[0].timeout_ms = 100U; }
    if (strcmp(mode, "test-timeout") == 0) strcpy(requests[0].name, "notes.timeout");
    if (strcmp(mode, "missing-build") == 0) strcpy(requests[0].build_directory, "/umicom/missing build");
    if (strcmp(mode, "missing-name") == 0) strcpy(requests[0].name, "notes.absent");
    if (strcmp(mode, "config") == 0) { strcpy(requests[0].name, "notes.config"); strcpy(requests[0].configuration, "Release"); }
    if (strcmp(mode, "two-roots") == 0) {
        count = 2U; requests[0] = Request(root, "notes.root", "one"); requests[1] = Request(other, "notes.root", "two");
    }
    if (strcmp(mode, "large-output") == 0) strcpy(requests[0].name, "notes.large");
    if (strcmp(mode, "cancel-active") == 0 || strcmp(mode, "partial-progress") == 0 || strcmp(mode, "unrelated-task") == 0) {
        count = 3U; requests[1] = Request(root, "notes.wait", "two"); requests[2] = Request(root, "notes.after", "three");
        RemoveMarker(root, "ready"); RemoveMarker(root, "after");
    }
    CHECK(UmiCtestJobCreate(requests, count, &options, &job) == UMI_STATUS_OK);
    CHECK(Snapshot(job).planned == count * (options.repeat_count ? options.repeat_count : 1U));
    if (strcmp(mode, "created") == 0) {
        UmiTestResult result, before; memset(&result, 0x5a, sizeof(result)); before = result;
        CHECK(Snapshot(job).state == UMI_TASK_CREATED);
        CHECK(UmiCtestJobResultAt(job, 0U, &result, NULL) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&result, &before, sizeof(result)) == 0);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(mode, "cancel-created") == 0) {
        CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
        CHECK(Snapshot(job).state == UMI_TASK_CANCELLED && Snapshot(job).completed == 0U);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(mode, "copied-request") == 0) memset(requests, 0, sizeof(requests));
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueConfig queueConfig = {1U, 1U};
    CHECK(umi_task_queue_create(&queueConfig, &queue) == UMI_STATUS_OK);
    if (strcmp(mode, "wrong-thread") == 0) {
        WrongThread test = {job, queue, 0}; UmiThread *thread = NULL; int code;
        CHECK(umi_thread_start(WrongOwner, &test, &thread) == UMI_STATUS_OK);
        CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK && test.passed);
        umi_thread_destroy(thread); CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK);
        umi_task_queue_destroy(queue); return 0;
    }
    UmiTask *blocker = NULL, *extra = NULL;
    atomic_int release = 0, executions = 0;
    if (strcmp(mode, "full-queue") == 0 || strcmp(mode, "cancel-queued") == 0) {
        UmiTaskConfig cfg = {0}; cfg.function = Block; cfg.user_data = &release; cfg.label = "blocker";
        CHECK(umi_task_create(&cfg, &blocker) == UMI_STATUS_OK);
        CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
        while (umi_task_state(blocker) != UMI_TASK_RUNNING) umi_thread_sleep_ms(1U);
        if (strcmp(mode, "full-queue") == 0) {
            cfg.function = Count; cfg.user_data = &executions;
            CHECK(umi_task_create(&cfg, &extra) == UMI_STATUS_OK);
            CHECK(umi_task_queue_submit(queue, extra) == UMI_STATUS_OK);
            CHECK(UmiCtestJobSubmit(job, queue) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(Snapshot(job).state == UMI_TASK_CREATED);
            atomic_store(&release, 1);
            CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
        }
    }
    CHECK(UmiCtestJobSubmit(job, queue) == UMI_STATUS_OK);
    if (strcmp(mode, "double-submit") == 0)
        CHECK(UmiCtestJobSubmit(job, queue) != UMI_STATUS_OK);
    if (strcmp(mode, "cancel-queued") == 0) {
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_BUSY);
        CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
        CHECK(Snapshot(job).completed == 0U);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK); job = NULL;
        atomic_store(&release, 1);
    }
    if (strcmp(mode, "cancel-active") == 0 || strcmp(mode, "partial-progress") == 0 || strcmp(mode, "unrelated-task") == 0) {
        AwaitMarker(root);
        CHECK(Snapshot(job).state == UMI_TASK_RUNNING && Snapshot(job).completed == 1U);
        CHECK(Result(job, 0U).state == UMI_TEST_STATE_PASSED);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_BUSY);
        if (strcmp(mode, "unrelated-task") == 0) {
            UmiTaskConfig cfg = {0}; cfg.label = "unrelated"; cfg.function = Count; cfg.user_data = &executions;
            CHECK(umi_task_create(&cfg, &extra) == UMI_STATUS_OK);
            CHECK(umi_task_queue_submit(queue, extra) == UMI_STATUS_OK);
        }
        CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
    }
    if (job != NULL) {
        UmiCtestJobSnapshot end = Wait(job);
        if (strcmp(mode, "cancel-active") == 0 || strcmp(mode, "partial-progress") == 0 || strcmp(mode, "unrelated-task") == 0) {
            CHECK(end.state == UMI_TASK_CANCELLED && end.completed == 2U && end.cancelled == 1U && end.passed == 1U);
            CHECK(!Exists(root, "after"));
        } else if (strcmp(mode, "continue-failure") == 0 || strcmp(mode, "stop-failure") == 0) {
            CHECK(end.state == UMI_TASK_FAILED && end.failed == 1U);
            CHECK(end.completed == (options.stop_on_failure ? 2U : 3U));
        } else if (strcmp(mode, "disabled") == 0 || strcmp(mode, "skip") == 0) {
            CHECK(end.skipped == 1U && end.passed == 0U && end.state == UMI_TASK_SUCCEEDED);
        } else if (strcmp(mode, "process-timeout") == 0 || strcmp(mode, "test-timeout") == 0) {
            CHECK(end.timed_out == 1U && end.state == UMI_TASK_FAILED);
        } else if (strcmp(mode, "missing-build") == 0 || strcmp(mode, "missing-name") == 0 || strcmp(mode, "config") == 0) {
            CHECK(end.not_run == 1U && end.passed == 0U && end.state == UMI_TASK_FAILED);
        } else if (strcmp(mode, "two-roots") == 0) {
            CHECK(end.completed == 2U && end.passed == 1U && end.failed == 1U);
        } else if (strcmp(mode, "large-output") == 0) {
            CHECK(end.failed == 1U && end.passed == 0U);
        } else {
            CHECK(end.passed == end.planned && end.completed == end.planned && end.state == UMI_TASK_SUCCEEDED);
        }
        if (strcmp(mode, "late-cancel") == 0) {
            CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
            UmiCtestJobSnapshot after = Snapshot(job);
            CHECK(after.state == end.state && after.completed == end.completed && after.passed == end.passed);
        }
        if (strcmp(mode, "repeat") == 0) {
            for (size_t i = 0U; i < 6U; ++i) {
                UmiTestResult r; uint32_t attempt = 0;
                CHECK(UmiCtestJobResultAt(job, i, &r, &attempt) == UMI_STATUS_OK);
                CHECK(strcmp(r.test_id, requests[i % 3U].test_id) == 0 && attempt == (uint32_t)(i / 3U) + 1U);
            }
        }
        if (strcmp(mode, "two-roots") == 0) {
            UmiTestResult a = Result(job, 0U), b = Result(job, 1U);
            CHECK(a.state == UMI_TEST_STATE_PASSED && b.state == UMI_TEST_STATE_FAILED && strstr(b.output, "root=B") != NULL);
        }
        if (strcmp(mode, "large-output") == 0) { UmiTestResult r = Result(job, 0U); CHECK(strstr(r.output, "NOTES OUTPUT TAIL") != NULL); }
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK);
    }
    CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
    if (strcmp(mode, "unrelated-task") == 0 || strcmp(mode, "full-queue") == 0) CHECK(atomic_load(&executions) == 1);
    umi_task_destroy(blocker); umi_task_destroy(extra); umi_task_queue_destroy(queue);
    return 0;
}
