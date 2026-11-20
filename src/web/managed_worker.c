/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/managed_worker.c
 *
 * PURPOSE:
 *   Manage an explicitly configured background process and a single-job FastCGI gate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/managed_worker.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>

struct UmiWebManagedWorker {
    UmiProcessSupervisor *supervisor;
    UmiProcessJobId job;
    bool confirmed;
    bool stopRequested;
};

static UmiStatus CurrentJob(const UmiWebManagedWorker *worker, UmiProcessJobId job)
{
    /* Stale route or operator activity cannot update another child. */
    if (worker == NULL || job == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return worker->job == job ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiWebManagedWorkerCreate(UmiWebManagedWorker **outWorker)
{
    if (outWorker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*outWorker != NULL) return UMI_STATUS_INVALID_STATE;
    UmiWebManagedWorker *worker = calloc(1U, sizeof *worker);
    if (worker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* A private single-slot supervisor enforces explicit result collection. */
    const UmiProcessSupervisorConfig config = {.capacity = 1U};
    UmiStatus status = umi_process_supervisor_create(&config, &worker->supervisor);
    if (status != UMI_STATUS_OK) {
        free(worker);
        return status;
    }
    *outWorker = worker;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebManagedWorkerStart(UmiWebManagedWorker *worker, const char *label,
    const UmiProcessRequest *request, UmiProcessJobId *outJob)
{
    if (worker == NULL || request == NULL || outJob == NULL || request->cancellation != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (worker->job != 0U) return UMI_STATUS_BUSY;
    /* These paths come from the deploying owner, independently of routing.
     * Keep shell parsing and inherited working-directory lookup out of startup. */
    if (UmiOutputFileValidatePath(request->program) != UMI_STATUS_OK ||
        UmiOutputFileValidatePath(request->working_directory) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProcessRequest background = *request;
    background.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiProcessJobId admitted = 0U;
    UmiStatus status = UmiProcessSupervisorSubmitWithLifetime(worker->supervisor, label,
        &background, UMI_PROCESS_LIFETIME_TREE, &admitted);
    if (status != UMI_STATUS_OK) return status;
    worker->job = admitted;
    worker->confirmed = false;
    worker->stopRequested = false;
    *outJob = admitted;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebManagedWorkerRead(const UmiWebManagedWorker *worker,
    UmiWebManagedWorkerSnapshot *outSnapshot)
{
    if (worker == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebManagedWorkerSnapshot snapshot = {0};
    if (worker->job != 0U) {
        /* Read state after creation. A terminal snapshot cannot inherit ready
         * from the previously running child, even when creation was observed. */
        int launched = 0;
        UmiStatus status = UmiProcessSupervisorLaunchObserved(worker->supervisor, worker->job, &launched);
        if (status == UMI_STATUS_OK)
            status = umi_process_supervisor_snapshot(worker->supervisor, worker->job, &snapshot.job);
        if (status != UMI_STATUS_OK) return status;
        snapshot.creationObserved = launched != 0;
        snapshot.stopRequested = worker->stopRequested;
        snapshot.ready = snapshot.creationObserved && snapshot.job.state == UMI_PROCESS_JOB_RUNNING &&
            worker->confirmed && !snapshot.stopRequested;
    }
    *outSnapshot = snapshot;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebManagedWorkerConfirmReady(UmiWebManagedWorker *worker, UmiProcessJobId job)
{
    UmiStatus status = CurrentJob(worker, job);
    if (status != UMI_STATUS_OK) return status;
    /* The caller supplies protocol evidence for this job, never a blanket
     * permission for whatever process happens to occupy the next launch. */
    UmiWebManagedWorkerSnapshot snapshot;
    status = UmiWebManagedWorkerRead(worker, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.job.state != UMI_PROCESS_JOB_RUNNING && snapshot.job.state != UMI_PROCESS_JOB_CREATED)
        return UMI_STATUS_UNAVAILABLE;
    if (snapshot.stopRequested || !snapshot.creationObserved || snapshot.job.state != UMI_PROCESS_JOB_RUNNING)
        return UMI_STATUS_BUSY;
    worker->confirmed = true;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebManagedWorkerRequestStop(UmiWebManagedWorker *worker, UmiProcessJobId job)
{
    UmiStatus status = CurrentJob(worker, job);
    if (status != UMI_STATUS_OK) return status;
    /* Withdraw the gate upon the stop request. Collection remains separate. */
    status = umi_process_supervisor_cancel(worker->supervisor, job);
    if (status == UMI_STATUS_OK) {
        worker->confirmed = false;
        worker->stopRequested = true;
    }
    return status;
}

UmiStatus UmiWebManagedWorkerCollect(UmiWebManagedWorker *worker, UmiProcessJobId job,
    UmiWebManagedWorkerSnapshot *outSnapshot)
{
    if (outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = CurrentJob(worker, job);
    if (status != UMI_STATUS_OK) return status;
    UmiWebManagedWorkerSnapshot snapshot;
    status = UmiWebManagedWorkerRead(worker, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.job.state == UMI_PROCESS_JOB_RUNNING || snapshot.job.state == UMI_PROCESS_JOB_CREATED)
        return UMI_STATUS_BUSY;
    /* Keep the terminal observation accessible if native release must retry. */
    status = UmiProcessSupervisorReleaseJob(worker->supervisor, job);
    if (status != UMI_STATUS_OK) return status;
    worker->job = 0U;
    worker->confirmed = false;
    worker->stopRequested = false;
    *outSnapshot = snapshot;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebManagedWorkerRelease(UmiWebManagedWorker **inOutWorker)
{
    if (inOutWorker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*inOutWorker == NULL) return UMI_STATUS_OK;
    UmiWebManagedWorker *worker = *inOutWorker;
    /* Preserve the entire owner until native stop and reaping both succeed. */
    UmiStatus status = UmiProcessSupervisorRelease(&worker->supervisor);
    if (status != UMI_STATUS_OK) return status;
    free(worker);
    *inOutWorker = NULL;
    return UMI_STATUS_OK;
}

static UmiStatus GateOpen(void *context, const UmiCancellationToken *cancel, UmiWebConnectionIo *out)
{
    const UmiWebManagedFastCgiGate *gate = context;
    if (gate == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = CurrentJob(gate->worker, gate->job);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    UmiWebManagedWorkerSnapshot snapshot;
    status = UmiWebManagedWorkerRead(gate->worker, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.job.state != UMI_PROCESS_JOB_RUNNING && snapshot.job.state != UMI_PROCESS_JOB_CREATED)
        return UMI_STATUS_UNAVAILABLE;
    if (!snapshot.ready) return UMI_STATUS_BUSY;
    /* A ready observation can still age. The real bounded endpoint connection
     * keeps its status and is never retried or used to launch a new worker. */
    UmiWebConnectionIo admitted = {0};
    status = gate->backend.open(gate->backend.context, cancel, &admitted);
    if (status == UMI_STATUS_OK) *out = admitted;
    return status;
}

static void GateClose(void *context, UmiWebConnectionIo *connection)
{
    const UmiWebManagedFastCgiGate *gate = context;
    /* Return an acquired lease to the same owner even after the child exits. */
    gate->backend.close(gate->backend.context, connection);
}

UmiStatus UmiWebManagedFastCgiBind(UmiWebManagedWorker *worker, UmiProcessJobId job,
    const UmiWebFastCgiConnector *backend, UmiWebManagedFastCgiGate *gate,
    UmiWebFastCgiConnector *outConnector)
{
    if (backend == NULL || backend->open == NULL || backend->close == NULL || gate == NULL || outConnector == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = CurrentJob(worker, job);
    if (status != UMI_STATUS_OK) return status;
    /* The registration explicitly borrows the chosen job and transport. A
     * restart needs a fresh binding after the owner's next endpoint check. */
    *gate = (UmiWebManagedFastCgiGate){.worker = worker, .job = job, .backend = *backend};
    *outConnector = (UmiWebFastCgiConnector){.context = gate, .open = GateOpen, .close = GateClose};
    return UMI_STATUS_OK;
}
