/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_reader.h
 * PURPOSE: Read saved test evidence on a task queue without blocking native event handling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_READER_H
#define UMICOM_TESTING_ARCHIVE_READER_H
#include "umicom/testing/archive.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiTestArchiveReadKind
    {
        UMI_TEST_ARCHIVE_READ_CATALOG,
        UMI_TEST_ARCHIVE_READ_ATTEMPT
    } UmiTestArchiveReadKind;
    typedef struct UmiTestArchiveReader UmiTestArchiveReader;
    typedef struct UmiTestArchiveReaderSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
        UmiTestArchiveReadKind kind;
        uint64_t id;
        size_t index;
        bool ready;
    } UmiTestArchiveReaderSnapshot;
    /** Copy a read request and borrow archive until Destroy succeeds. Catalogue
     * reads require id/index zero. Attempt reads require a nonzero id and a
     * zero-based completed-attempt index. Creation allocates bounded result storage
     * on the heap but performs no database I/O. *out_reader is cleared on error.
     * Create, Submit and Destroy belong to the creating thread. */
    UmiStatus UmiTestArchiveReaderCreate(UmiTestArchive *archive, UmiTestArchiveReadKind kind,
                                         uint64_t id, size_t index,
                                         UmiTestArchiveReader **out_reader);
    /** Submit once; queue rejection leaves a created reader available to retry.
     * The queue must remain alive until its owner drains or shuts it down. */
    UmiStatus UmiTestArchiveReaderSubmit(UmiTestArchiveReader *reader, UmiTaskQueue *queue);
    /** Request cancellation without waiting. A completed copied observation remains
     * readable after a late Stop. Cancellation is checked around catalogue I/O. */
    UmiStatus UmiTestArchiveReaderCancel(UmiTestArchiveReader *reader);
    /** Copy state without database I/O. ready is true only for a complete copied
     * observation; task cancellation alone does not erase a completed read. */
    UmiStatus UmiTestArchiveReaderRead(UmiTestArchiveReader *reader,
                                       UmiTestArchiveReaderSnapshot *out_snapshot);
    /** Copy an already-read catalogue. NOT_FOUND means no complete observation.
     * The wrong read kind returns INVALID_STATE. Errors preserve out_catalog.
     * Keep reader alive during this and all other snapshot/copy operations. */
    UmiStatus UmiTestArchiveReaderCatalog(UmiTestArchiveReader *reader,
                                          UmiTestArchiveCatalog *out_catalog);
    /** Copy an already-read attempt, including its accepted origin and request.
     * This reads memory only, follows the same error rules as ReaderCatalog, and
     * never restores historical results into a live test registry. */
    UmiStatus UmiTestArchiveReaderAttempt(UmiTestArchiveReader *reader,
                                          UmiTestArchiveAttempt *out_attempt);
    /** Release a created or terminal reader. BUSY preserves a queued/running reader.
     * Cancel, keep polling, then destroy. NULL is accepted. No method joins a worker. */
    UmiStatus UmiTestArchiveReaderDestroy(UmiTestArchiveReader *reader);
#ifdef __cplusplus
}
#endif
#endif
