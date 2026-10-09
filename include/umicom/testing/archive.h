/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive.h
 * PURPOSE: Retain immutable completed CTest selections in a private local Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_H
#define UMICOM_TESTING_ARCHIVE_H
#include "umicom/data/data_server.h"
#include "umicom/data/job_identity.h"
#include "umicom/testing/ctest_capture.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_TEST_ARCHIVE_CAPACITY 32U
#define UMI_TEST_ARCHIVE_REVISION_CAPACITY 129U
    typedef struct UmiTestArchive UmiTestArchive;
    typedef struct UmiTestArchiveOrigin
    {
        char source_root[UMI_CTEST_JOB_PATH_CAPACITY];
        /* Optional caller-supplied revision description. Empty means not captured.
     * Neither this description nor workspace_generation proves a clean checkout
     * or identifies every source byte used to build a test executable. */
        char source_revision[UMI_TEST_ARCHIVE_REVISION_CAPACITY];
        uint64_t workspace_generation;
        bool retain_output;
        /* Copy this evidence when the run is accepted, not when Save is clicked.
         * Empty identity keeps legacy callers valid. Inputs remain optional and
         * describe only the caller's documented input set, not every build file. */
        UmiJobIdentity identity;
    } UmiTestArchiveOrigin;
    typedef struct UmiTestArchiveEntry
    {
        uint64_t id;
        UmiTestArchiveOrigin origin;
        UmiCtestJobPlanSnapshot plan;
        UmiCtestJobSnapshot run;
        /* Computed from the actual ordered requests during the save transaction.
         * Empty means an older record without this evidence. It is not a source
         * fingerprint or proof that the recorded test binary is still current. */
        char selection_digest[UMI_JOB_IDENTITY_DIGEST_CAPACITY];
    } UmiTestArchiveEntry;
    typedef struct UmiTestArchiveCatalog
    {
        size_t count;
        UmiTestArchiveEntry entries[UMI_TEST_ARCHIVE_CAPACITY];
    } UmiTestArchiveCatalog;
    /* The archive borrows its Data Server. Quiesce callers before destroying either.
 * scope is a nonempty ASCII key (letters/digits/dot/dash/underscore, <=64 bytes).
 * Calls own short transactions and must not run inside a caller transaction.
 * SQLite is persistent; memory storage is transient. The archive is not encrypted,
 * a credential vault, or an execution authority. Use a protected local database.
 * The memory backend has a shared record limit and may fill before this
 * archive reaches its run limit. Capacity errors roll back without eviction.
 * Each operation returns BUSY rather than taking another transaction's ownership. */
    UmiStatus UmiTestArchiveCreate(UmiDataServer *server, const char *scope, UmiTestArchive **out_archive);
    void UmiTestArchiveDestroy(UmiTestArchive *archive);
    /* Save one terminal job, its complete selection and every completed result in
 * one transaction. Queued/running jobs return BUSY. Attempts that never started
 * remain explicit in planned-completed; they are never invented as passed rows.
 * No eviction, launch, replay or mutation of the source job occurs. Keep source
 * and archive alive through the call. Cancellation rolls back before publication.
 * Output retention is explicit; false saves outcomes without diagnostic text.
 * True retains the adapter's bounded diagnostic tail, not a complete process log.
 * Names and paths may also contain private information. No redaction is promised.
 * Saving can involve many records; use the archive_write.h worker from interactive hosts.
 * Source/origin/output storage must be independent. Errors leave out_entry intact. */
    UmiStatus UmiTestArchiveSave(UmiTestArchive *archive, UmiCtestJob *source,
                                 const UmiTestArchiveOrigin *origin, const UmiCancellationToken *cancellation,
                                 UmiTestArchiveEntry *out_entry);
    /* Read history without applying it to a current catalogue. Results remain tied
 * to their recorded selection even when another workspace is open. Catalogue
 * order is increasing durable ID; IDs are never reused after explicit removal.
 * Allocate catalogues/results on the heap when frontend stack space is limited.
 * Errors, missing records and corrupt encodings preserve output objects. */
    UmiStatus UmiTestArchiveList(UmiTestArchive *archive, UmiTestArchiveCatalog *out_catalog);
    UmiStatus UmiTestArchiveRead(UmiTestArchive *archive, uint64_t id, UmiTestArchiveEntry *out_entry);
    UmiStatus UmiTestArchiveRequestAt(UmiTestArchive *archive, uint64_t id, size_t index,
                                      UmiCtestJobRequest *out_request);
    UmiStatus UmiTestArchiveResultAt(UmiTestArchive *archive, uint64_t id, size_t index,
                                     UmiTestResult *out_result, uint32_t *out_attempt);
    /** One completed attempt and its origin from a single database observation.
     * This object contains a diagnostic tail; allocate it on the heap when the
     * host has a small thread stack. It owns copies and borrows no archive data. */
    typedef struct UmiTestArchiveAttempt
    {
        UmiTestArchiveEntry entry;
        UmiCtestJobRequest request;
        UmiTestResult result;
        uint32_t attempt;
    } UmiTestArchiveAttempt;
    /** Read one completed attempt under one transaction. index is zero-based in
     * recorded attempt order. Missing/corrupt records and cancellation preserve
     * out_attempt. This performs storage I/O; interactive hosts should use the
     * archive_reader.h worker. The output must not alias archive storage. */
    UmiStatus UmiTestArchiveReadAttempt(UmiTestArchive *archive, uint64_t id, size_t index,
        const UmiCancellationToken *cancellation, UmiTestArchiveAttempt *out_attempt);
    /* Explicitly remove exactly one saved run and its owned rows atomically.
 * This never touches source files, test executables or another archive scope. */
    UmiStatus UmiTestArchiveRemove(UmiTestArchive *archive, uint64_t id);
    /** Remove one saved run with cooperative cancellation before transaction commit.
     * Cancellation rolls back deleted rows. A Stop request received after commit
     * does not reverse removal: OK still means the transaction committed. NULL
     * cancellation retains the synchronous contract. This performs storage I/O;
     * interactive hosts should use archive_removal.h to keep their event loop free. */
    UmiStatus UmiTestArchiveRemoveWithCancellation(UmiTestArchive *archive, uint64_t id,
        const UmiCancellationToken *cancellation);

#ifdef __cplusplus
}
#endif
#endif
