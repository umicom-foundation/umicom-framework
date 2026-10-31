/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/local_replace.h
 *
 * PURPOSE:
 *   Replace owned local files through exclusive staging and report publication failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_LOCAL_REPLACE_H
#define UMICOM_PLATFORM_LOCAL_REPLACE_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiLocalFileReplaceStage
    {
        UMI_LOCAL_REPLACE_VALIDATE,
        UMI_LOCAL_REPLACE_DESTINATION,
        UMI_LOCAL_REPLACE_CREATE,
        UMI_LOCAL_REPLACE_WRITE,
        UMI_LOCAL_REPLACE_CLOSE,
        UMI_LOCAL_REPLACE_RECHECK,
        UMI_LOCAL_REPLACE_PUBLISH,
        UMI_LOCAL_REPLACE_COMPLETE
    } UmiLocalFileReplaceStage;

    /* A caller-owned diagnostic record contains no file contents or paths.
     * stage identifies the failing operation, or COMPLETE on success.
     * last_publish_error is the most recent failed MoveFileExW/GetLastError or
     * POSIX rename/errno value, including a failure recovered by retry. Zero
     * means no publication error was captured; other stages do not guess a
     * native error from stale thread state. Cleanup never replaces this value. */
    typedef struct UmiLocalFileReplaceReport
    {
        UmiLocalFileReplaceStage stage;
        UmiStatus status;
        uint32_t last_publish_error;
        unsigned publish_attempts;
        bool cleanup_attempted;
        bool cleanup_succeeded;
    } UmiLocalFileReplaceReport;

    /* Write a complete replacement beside an absolute destination, flush it, then
 * rename it into place. The trusted parent must already exist; this operation
 * creates no directories. Path syntax follows UmiOutputFileValidatePath.
 * An existing destination must be a regular file, not a link or directory.
 * Exclusive staging avoids clobbering another writer's temporary file. Windows
 * paths are UTF-8; POSIX paths use native bytes. Empty content is supported.
 * The old destination remains until publication succeeds. This replaces its
 * metadata/permissions with the staging file's; it is intended for owned local
 * settings, not an editor save preserving arbitrary document ACLs or streams.
 * Calls may run concurrently, but writers to one destination are last-writer
 * wins. Parent aliases and hostile directory changes are not confined. File
 * flush and rename are not a guarantee against power loss; no parent-directory
 * durability barrier is issued. Failed cleanup can leave an owned staging file.
 * The established umi_atomic_file_write contract remains available unchanged. */
    UmiStatus UmiLocalFileReplace(const char *path, const void *bytes, size_t size);

    /* The same operation with an optional diagnostic result, overwritten on
     * every return. Windows publication retries only sharing/lock violations,
     * at most eight attempts with 280 ms of requested waits in total. Native
     * storage calls can take longer. Access denial is never bypassed or retried.
     * A changed destination is rechecked before each retry. A caller should run
     * this blocking operation on its storage worker, not its UI event thread. */
    UmiStatus UmiLocalFileReplaceDetailed(const char *path, const void *bytes, size_t size,
                                         UmiLocalFileReplaceReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
