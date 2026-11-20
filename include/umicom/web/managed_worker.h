/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/managed_worker.h
 *
 * PURPOSE:
 *   Own one explicitly started local web worker and bind a route to its ready lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_MANAGED_WORKER_H
#define UMICOM_WEB_MANAGED_WORKER_H

#include "umicom/platform/process_supervisor.h"
#include "umicom/web/fastcgi_application.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebManagedWorker UmiWebManagedWorker;

typedef struct UmiWebManagedWorkerSnapshot {
    UmiProcessJobSnapshot job;
    bool creationObserved;
    bool ready;
    bool stopRequested;
} UmiWebManagedWorkerSnapshot;

/* Serialize all operations and route callbacks on this worker's owner. The
 * supervisor owns exactly one job and its native process tree. Start accepts
 * trusted, named absolute native program and working-directory paths with
 * explicit argument vectors. The request is copied and the background window
 * is hidden. Leave request->cancellation NULL and control cancellation through
 * the returned job ID. HTTP request data must never choose the program,
 * arguments, configuration or a restart. */
UmiStatus UmiWebManagedWorkerCreate(UmiWebManagedWorker **outWorker);
UmiStatus UmiWebManagedWorkerStart(UmiWebManagedWorker *worker, const char *label,
    const UmiProcessRequest *request, UmiProcessJobId *outJob);

/* An idle snapshot has job ID zero. A successful native creation does not
 * establish protocol readiness. The owner confirms readiness for the exact
 * job after checking its configured endpoint. Exiting or stopping a job
 * withdraws readiness. Snapshots can age before the next I/O operation. */
UmiStatus UmiWebManagedWorkerRead(const UmiWebManagedWorker *worker,
    UmiWebManagedWorkerSnapshot *outSnapshot);
UmiStatus UmiWebManagedWorkerConfirmReady(UmiWebManagedWorker *worker, UmiProcessJobId job);

/* RequestStop withdraws readiness immediately but leaves completion observable.
 * Collect returns BUSY while work remains; after a terminal result it releases
 * the job and publishes its snapshot. A failed release leaves the slot for
 * retry. Start returns BUSY until the previous job is collected. */
UmiStatus UmiWebManagedWorkerRequestStop(UmiWebManagedWorker *worker, UmiProcessJobId job);
UmiStatus UmiWebManagedWorkerCollect(UmiWebManagedWorker *worker, UmiProcessJobId job,
    UmiWebManagedWorkerSnapshot *outSnapshot);

/* Detach routes and finish callbacks before releasing their owner. Release can
 * block while reaping the managed tree. Failure keeps the pointer for retry.
 * Collect beforehand if the terminal result must be retained. */
UmiStatus UmiWebManagedWorkerRelease(UmiWebManagedWorker **inOutWorker);

typedef struct UmiWebManagedFastCgiGate {
    UmiWebManagedWorker *worker;
    UmiProcessJobId job;
    UmiWebFastCgiConnector backend;
} UmiWebManagedFastCgiGate;

/* Build a connector that borrows an owner-approved backend and one exact job.
 * During start/stop it returns BUSY, after exit it returns UNAVAILABLE, and a
 * replaced job makes the old gate stale (NOT_FOUND). It never starts or retries
 * a worker. A successful open delegates its single close to the backend even
 * if the child later exits. Keep gate, worker and backend storage alive and
 * distinct while registered. Detach the registration and finish its leases
 * before reusing gate storage. Use the existing route's normal failure policy. */
UmiStatus UmiWebManagedFastCgiBind(UmiWebManagedWorker *worker, UmiProcessJobId job,
    const UmiWebFastCgiConnector *backend, UmiWebManagedFastCgiGate *gate,
    UmiWebFastCgiConnector *outConnector);

#ifdef __cplusplus
}
#endif
#endif
