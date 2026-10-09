/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_removal.h
 * PURPOSE: Remove a saved test run on a shared queue with an explicit committed outcome.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_REMOVAL_H
#define UMICOM_TESTING_ARCHIVE_REMOVAL_H
#include "umicom/testing/archive.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiTestArchiveRemoval UmiTestArchiveRemoval;
    typedef struct UmiTestArchiveRemovalSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
        uint64_t id;
        /* True only when storage committed removal. It remains true after late Stop. */
        bool committed;
    } UmiTestArchiveRemovalSnapshot;
    /** Copy one exact, nonzero run ID and borrow its archive until Destroy succeeds.
     * Creation performs no storage I/O. Create, Submit and Destroy belong to the
     * creating thread. Keep the archive and its Data Server alive through completion.
     * Failure clears out_removal. This never removes the database file itself. */
    UmiStatus UmiTestArchiveRemovalCreate(UmiTestArchive *archive, uint64_t id,
                                          UmiTestArchiveRemoval **out_removal);
    /** Queue the copied request once. A rejected submission leaves it unexecuted. */
    UmiStatus UmiTestArchiveRemovalSubmit(UmiTestArchiveRemoval *removal, UmiTaskQueue *queue);
    /** Request cooperative Stop. Read committed before describing the outcome:
     * cancellation does not undo a transaction which already committed. */
    UmiStatus UmiTestArchiveRemovalCancel(UmiTestArchiveRemoval *removal);
    /** Copy progress from memory without waiting for storage. Concurrent reads require
     * the caller to keep the removal alive. Invalid arguments preserve out_snapshot. */
    UmiStatus UmiTestArchiveRemovalRead(UmiTestArchiveRemoval *removal,
                                        UmiTestArchiveRemovalSnapshot *out_snapshot);
    /** Release a created or terminal request on its creating thread. BUSY preserves a
     * queued/running request; cancel and poll it before retrying. NULL is accepted. */
    UmiStatus UmiTestArchiveRemovalDestroy(UmiTestArchiveRemoval *removal);
#ifdef __cplusplus
}
#endif
#endif
