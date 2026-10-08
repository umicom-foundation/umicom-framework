/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_write.h
 * PURPOSE: Save test evidence on a shared task queue while a frontend remains responsive.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_WRITE_H
#define UMICOM_TESTING_ARCHIVE_WRITE_H
#include "umicom/testing/archive.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiTestArchiveWrite UmiTestArchiveWrite;
    typedef struct UmiTestArchiveWriteSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
        /* id != 0 is the committed database outcome, even if Stop arrived just
     * after commit. A cancelled task alone is not proof that nothing was saved. */
        UmiTestArchiveEntry saved;
    } UmiTestArchiveWriteSnapshot;
    /* Creation copies origin and borrows a terminal source job and archive. Keep
 * both alive until the writer is destroyed. Create, Submit and Destroy belong
 * to the creating thread. No method joins the worker or changes test outcomes.
 * Frontends should deny replacing the source job while this writer is pending.
 * out_writer is cleared on error. The source must already be terminal. */
    UmiStatus UmiTestArchiveWriteCreate(UmiTestArchive *archive, UmiCtestJob *source,
                                        const UmiTestArchiveOrigin *origin, UmiTestArchiveWrite **out_writer);
    UmiStatus UmiTestArchiveWriteSubmit(UmiTestArchiveWrite *writer, UmiTaskQueue *queue);
    UmiStatus UmiTestArchiveWriteCancel(UmiTestArchiveWrite *writer);
    /* Read copies memory only; it does not touch SQLite. Keep the writer alive
 * throughout concurrent reads. Errors preserve the caller's snapshot. */
    UmiStatus UmiTestArchiveWriteRead(UmiTestArchiveWrite *writer, UmiTestArchiveWriteSnapshot *out_snapshot);
    /* BUSY preserves a queued/running writer. Cancel, keep polling, then destroy.
 * NULL is accepted. The queue may retain its task after callback completion,
 * but a terminal task will not access this payload again. */
    UmiStatus UmiTestArchiveWriteDestroy(UmiTestArchiveWrite *writer);
#ifdef __cplusplus
}
#endif
#endif
