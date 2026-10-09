/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_open.h
 * PURPOSE: Open and validate a private test archive on a worker before transferring ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_OPEN_H
#define UMICOM_TESTING_ARCHIVE_OPEN_H
#include "umicom/platform/path.h"
#include "umicom/testing/archive.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiTestArchiveOpen UmiTestArchiveOpen;
    typedef struct UmiTestArchiveOpenSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
        char path[UMI_PATH_CAPACITY];
        bool ready;
        bool taken;
        bool stop_requested;
    } UmiTestArchiveOpenSnapshot;
    /** Copy an absolute local filename and archive scope without opening storage.
     * Scope follows ArchiveCreate's ASCII rules. The worker opens SQLite and reads
     * the bounded catalogue before publishing an attachment. Paths are not a
     * confinement boundary: choose a private directory whose aliases you trust.
     * A new database may be created even if a later validation or Stop prevents
     * attachment. No file is removed. *out_open is cleared on error. */
    UmiStatus UmiTestArchiveOpenCreate(const char *path, const char *scope,
                                       UmiTestArchiveOpen **out_open);
    /** Submit once from the creating thread. A rejected queue leaves the request
     * created and available for retry. The queue must outlive pending work. */
    UmiStatus UmiTestArchiveOpenSubmit(UmiTestArchiveOpen *opening, UmiTaskQueue *queue);
    /** Request Stop without joining the worker. Stop also prevents Take after a
     * successful read that has not yet been attached. An already taken attachment
     * remains owned by its recipient; cancellation cannot revoke that ownership. */
    UmiStatus UmiTestArchiveOpenCancel(UmiTestArchiveOpen *opening);
    /** Copy progress from memory, without opening or querying the database. Keep
     * opening alive during this call and any other concurrent observation. */
    UmiStatus UmiTestArchiveOpenRead(UmiTestArchiveOpen *opening,
                                     UmiTestArchiveOpenSnapshot *out_snapshot);
    /** Transfer both handles once, on the creating thread after task completion.
     * Both output pointers must address independent NULL handles. Errors preserve
     * them. The recipient must quiesce archive users, destroy the archive, then
     * destroy its Data Server. Success performs no database operation. */
    UmiStatus UmiTestArchiveOpenTake(UmiTestArchiveOpen *opening, UmiDataServer **out_database,
                                     UmiTestArchive **out_archive);
    /** Destroy on the creating thread. BUSY leaves queued/running work intact.
     * Unclaimed handles are closed; this can perform storage cleanup. NULL is
     * accepted. This never waits for a worker and never deletes a database file. */
    UmiStatus UmiTestArchiveOpenDestroy(UmiTestArchiveOpen *opening);
#ifdef __cplusplus
}
#endif
#endif
