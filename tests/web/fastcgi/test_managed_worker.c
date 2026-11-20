/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/fastcgi/test_managed_worker.c
 *
 * PURPOSE:
 *   Verify native worker lifecycle and exact-job admission through a FastCGI connector.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/clock.h"
#include "umicom/platform/threading.h"
#include "umicom/web/managed_worker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

typedef struct Backend {
    UmiStatus openStatus;
    unsigned opens;
    unsigned closes;
} Backend;

static UmiStatus Open(void *context, const UmiCancellationToken *cancel, UmiWebConnectionIo *out)
{
    Backend *backend = context;
    /* Count actual delegation separately from the status seen at the gate. */
    CHECK(!umi_cancellation_token_is_requested(cancel));
    ++backend->opens;
    out->context = backend;
    return backend->openStatus;
}

static void Close(void *context, UmiWebConnectionIo *connection)
{
    Backend *backend = context;
    /* Route leases must be returned to the transport that issued them. */
    CHECK(connection->context == backend);
    ++backend->closes;
}

static UmiWebManagedWorkerSnapshot WaitFor(UmiWebManagedWorker *worker, UmiProcessJobId job,
    UmiProcessJobState state, const char *marker)
{
    /* A clock limits the wait, while observed native state decides success. */
    UmiClock clock = umi_clock_system();
    const uint64_t start = clock.monotonic_nanoseconds(&clock);
    do {
        UmiWebManagedWorkerSnapshot snapshot;
        CHECK(UmiWebManagedWorkerRead(worker, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.job.job_id == job);
        if (snapshot.job.state == state && (marker == NULL ||
            (snapshot.creationObserved && strstr(snapshot.job.output, marker) != NULL))) return snapshot;
        CHECK(snapshot.job.state == UMI_PROCESS_JOB_CREATED || snapshot.job.state == UMI_PROCESS_JOB_RUNNING);
        umi_thread_sleep_ms(10U);
    } while (clock.monotonic_nanoseconds(&clock) - start < UINT64_C(7000000000));
    CHECK(false);
    return (UmiWebManagedWorkerSnapshot){0};
}

static void RejectInvalid(UmiWebManagedWorker *worker, const UmiProcessRequest *request)
{
    /* No rejected request may submit a child or publish a new job identity. */
    UmiProcessJobId out = 99U;
    UmiProcessRequest invalid = *request;
    invalid.program = "relative-program";
    CHECK(UmiWebManagedWorkerStart(worker, "invalid program", &invalid, &out) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = *request;
    invalid.working_directory = NULL;
    CHECK(UmiWebManagedWorkerStart(worker, "missing directory", &invalid, &out) == UMI_STATUS_INVALID_ARGUMENT);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    invalid = *request;
    invalid.cancellation = cancel;
    CHECK(UmiWebManagedWorkerStart(worker, "borrowed cancellation", &invalid, &out) == UMI_STATUS_INVALID_ARGUMENT);
    umi_cancellation_token_destroy(cancel);
    CHECK(out == 99U);
    UmiWebManagedWorkerSnapshot snapshot;
    CHECK(UmiWebManagedWorkerRead(worker, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.job.job_id == 0U && !snapshot.creationObserved && !snapshot.ready && !snapshot.stopRequested);
    CHECK(UmiWebManagedWorkerConfirmReady(worker, 99U) == UMI_STATUS_NOT_FOUND);
}

static void Finish(UmiWebManagedWorker *worker, UmiProcessJobId job)
{
    /* Observe and collect cancellation without confusing it with a stop request. */
    CHECK(UmiWebManagedWorkerRequestStop(worker, job) == UMI_STATUS_OK);
    UmiWebManagedWorkerSnapshot stopped = WaitFor(worker, job, UMI_PROCESS_JOB_CANCELLED, "managed-worker-running");
    CHECK(stopped.stopRequested && !stopped.ready);
    CHECK(UmiWebManagedWorkerCollect(worker, job, &stopped) == UMI_STATUS_OK);
    CHECK(stopped.job.job_id == job && stopped.stopRequested && stopped.job.state == UMI_PROCESS_JOB_CANCELLED);
}

static void CheckGate(UmiWebManagedWorker *worker, const UmiProcessRequest *request, const char *mode)
{
    Backend backend = {0};
    const UmiWebFastCgiConnector transport = {.context = &backend, .open = Open, .close = Close};
    UmiWebManagedFastCgiGate gate;
    UmiWebFastCgiConnector connector;
    UmiProcessJobId job = 0U;
    CHECK(UmiWebManagedWorkerStart(worker, "background fixture", request, &job) == UMI_STATUS_OK);
    CHECK(UmiWebManagedFastCgiBind(worker, job, &transport, &gate, &connector) == UMI_STATUS_OK);
    UmiWebManagedWorkerSnapshot snapshot = WaitFor(worker, job, UMI_PROCESS_JOB_RUNNING, "managed-worker-running");
    CHECK(!snapshot.ready);
    /* A real running child still cannot receive a route until confirmed. */
    UmiWebConnectionIo lease = {.context = worker};
    CHECK(connector.open(connector.context, NULL, &lease) == UMI_STATUS_BUSY);
    CHECK(lease.context == worker && backend.opens == 0U);
    CHECK(UmiWebManagedWorkerConfirmReady(worker, job + 1U) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiWebManagedWorkerCollect(worker, job, &snapshot) == UMI_STATUS_BUSY);
    CHECK(snapshot.job.job_id == job && snapshot.job.state == UMI_PROCESS_JOB_RUNNING);
    UmiProcessJobId refused = 99U;
    CHECK(UmiWebManagedWorkerStart(worker, "overlap", request, &refused) == UMI_STATUS_BUSY && refused == 99U);
    CHECK(UmiWebManagedWorkerConfirmReady(worker, job) == UMI_STATUS_OK);
    CHECK(UmiWebManagedWorkerRead(worker, &snapshot) == UMI_STATUS_OK && snapshot.ready);

    if (strcmp(mode, "downstream") == 0) {
        /* Transport refusal gets one attempt and preserves caller storage. */
        backend.openStatus = UMI_STATUS_TIMEOUT;
        CHECK(connector.open(connector.context, NULL, &lease) == UMI_STATUS_TIMEOUT);
        CHECK(lease.context == worker && backend.opens == 1U && backend.closes == 0U);
        Finish(worker, job);
        return;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    CHECK(connector.open(connector.context, cancel, &lease) == UMI_STATUS_CANCELLED && backend.opens == 0U);
    umi_cancellation_token_destroy(cancel);
    CHECK(connector.open(connector.context, NULL, &lease) == UMI_STATUS_OK);
    CHECK(backend.opens == 1U && lease.context == &backend);
    if (strcmp(mode, "ready") == 0) {
        /* The usual lifecycle returns the lease before collecting the child. */
        connector.close(connector.context, &lease);
        CHECK(backend.closes == 1U);
        Finish(worker, job);
        return;
    }
    /* Keep one existing lease while closing admission. Stopping a child must
     * not prevent returning the original lease to its backend. */
    CHECK(UmiWebManagedWorkerRequestStop(worker, job) == UMI_STATUS_OK);
    CHECK(UmiWebManagedWorkerRead(worker, &snapshot) == UMI_STATUS_OK && snapshot.stopRequested && !snapshot.ready);
    UmiWebConnectionIo rejected = {.context = worker};
    UmiStatus status = connector.open(connector.context, NULL, &rejected);
    CHECK((status == UMI_STATUS_BUSY || status == UMI_STATUS_UNAVAILABLE) && rejected.context == worker);
    connector.close(connector.context, &lease);
    CHECK(backend.opens == 1U && backend.closes == 1U);
    snapshot = WaitFor(worker, job, UMI_PROCESS_JOB_CANCELLED, "managed-worker-running");
    CHECK(connector.open(connector.context, NULL, &rejected) == UMI_STATUS_UNAVAILABLE);
    CHECK(UmiWebManagedWorkerStart(worker, "uncollected result", request, &refused) == UMI_STATUS_BUSY);
    CHECK(UmiWebManagedWorkerCollect(worker, job, &snapshot) == UMI_STATUS_OK);
    UmiProcessJobId next = 0U;
    CHECK(UmiWebManagedWorkerStart(worker, "explicit restart", request, &next) == UMI_STATUS_OK && next != job);
    snapshot = WaitFor(worker, next, UMI_PROCESS_JOB_RUNNING, "managed-worker-running");
    CHECK(!snapshot.ready && UmiWebManagedWorkerConfirmReady(worker, next) == UMI_STATUS_OK);
    CHECK(connector.open(connector.context, NULL, &rejected) == UMI_STATUS_NOT_FOUND && backend.opens == 1U);
    Finish(worker, next);
}

static void CheckTerminal(UmiWebManagedWorker *worker, UmiProcessRequest request, const char *mode)
{
    const bool missing = strcmp(mode, "missing") == 0;
    char program[2048];
    if (missing) {
        /* Start can admit configuration even if the OS later rejects creation. */
        int count = snprintf(program, sizeof program, "%s/umicom-unavailable-worker-fixture", request.working_directory);
        CHECK(count > 0 && count < (int)sizeof program);
        request.program = program;
    }
    const char *arguments[] = {"--child", "exit"};
    request.arguments = arguments;
    UmiProcessJobId job = 0U;
    CHECK(UmiWebManagedWorkerStart(worker, "terminal fixture", &request, &job) == UMI_STATUS_OK);
    UmiWebManagedWorkerSnapshot snapshot = WaitFor(worker, job, UMI_PROCESS_JOB_FAILED,
        missing ? NULL : "managed-worker-exit");
    CHECK(snapshot.creationObserved == !missing && !snapshot.stopRequested && !snapshot.ready);
    if (!missing) CHECK(snapshot.job.exit_code == 7);
    CHECK(UmiWebManagedWorkerConfirmReady(worker, job) == UMI_STATUS_UNAVAILABLE);
    CHECK(UmiWebManagedWorkerCollect(worker, job, &snapshot) == UMI_STATUS_OK && snapshot.job.job_id == job);
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "--child") == 0) {
        /* Use a known executable and an explicit lifetime cap even if a
         * regression prevents the owner from issuing a stop request. */
        const bool exits = strcmp(argv[2], "exit") == 0;
        puts(exits ? "managed-worker-exit" : "managed-worker-running");
        CHECK(fflush(stdout) == 0);
        if (!exits) umi_thread_sleep_ms(14000U);
        return exits ? 7 : 0;
    }
    CHECK(argc == 4);
    const char *mode = argv[1];
    const char *arguments[] = {"--child", "hold"};
    const UmiProcessRequest request = {.program = argv[2], .working_directory = argv[3],
        .arguments = arguments, .argument_count = 2U, .capture_stdout = 1, .timeout_ms = 12000U};
    UmiWebManagedWorker *worker = NULL;
    CHECK(UmiWebManagedWorkerCreate(&worker) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid") == 0) RejectInvalid(worker, &request);
    else if (strcmp(mode, "exit") == 0 || strcmp(mode, "missing") == 0) CheckTerminal(worker, request, mode);
    else if (strcmp(mode, "release") == 0) {
        UmiProcessJobId job = 0U;
        CHECK(UmiWebManagedWorkerStart(worker, "release fixture", &request, &job) == UMI_STATUS_OK);
        (void)WaitFor(worker, job, UMI_PROCESS_JOB_RUNNING, "managed-worker-running");
        /* The release path must stop and reap a still-running owned child. */
        CHECK(UmiWebManagedWorkerRelease(&worker) == UMI_STATUS_OK && worker == NULL);
    } else {
        CHECK(strcmp(mode, "ready") == 0 || strcmp(mode, "downstream") == 0 || strcmp(mode, "stop-and-restart") == 0);
        CheckGate(worker, &request, mode);
    }
    CHECK(UmiWebManagedWorkerRelease(&worker) == UMI_STATUS_OK && worker == NULL);
    return 0;
}
