/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/rooted_files.h
 *
 * PURPOSE:
 *   Provide bounded file access beneath an explicitly chosen local workspace root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_ROOTED_FILES_H
#define UMICOM_PLATFORM_ROOTED_FILES_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* These operations accept an ordinary absolute root and a portable relative
 * path. Parents must exist; .git, traversal, device names and ambiguous names
 * are refused. The root and descendants must not be links/reparse points.
 * Ancestors above the chosen root are trusted. No workspace trust or approval
 * is granted here: the calling project/patch service owns those decisions.
 * Calls may block and belong on a worker in interactive applications.
 *
 * POSIX operations use opened directory descriptors. Windows retains directory
 * handles without delete sharing for the call. These handles reduce path races;
 * this is not isolation from privileged processes or a content-revision lock.
 * The caller must serialize its own edits. Concurrent external leaf changes can
 * still make a prior read stale. No operation recursively removes a directory.
 */

    /* Check the same bounded root and relative-name syntax used by storage
 * operations, without opening a handle or resolving the working directory.
 * Success is syntax admission only: links, existence and access are checked
 * by the later operation. No output path or authority is manufactured. */
    UmiStatus UmiRootedFileValidatePath(const char *root, const char *relativePath);


    /** Apply the same portable relative-name syntax without requiring a root.
     * No file is opened. The caller supplies readable terminated input. */
    UmiStatus UmiRootedFileValidateRelativePath(const char *relativePath);

    typedef enum UmiRootedPathRelation {
        UMI_ROOTED_PATH_DISJOINT = 0,
        UMI_ROOTED_PATH_EQUAL = 1,
        UMI_ROOTED_PATH_FIRST_PARENT = 2,
        UMI_ROOTED_PATH_SECOND_PARENT = 3
    } UmiRootedPathRelation;

    /** Compare two admitted portable names after separator normalization and
     * ASCII-only case folding. Parent relations require a component boundary.
     * The result is unchanged on refusal. This conservative portability policy
     * does not resolve filesystem objects, Unicode case folding, links, 8.3
     * names or mounts. Serialize access to input storage for the whole call. */
    UmiStatus UmiRootedFileCompareRelativePaths(const char *first, const char *second,
        UmiRootedPathRelation *outRelation);


    typedef struct UmiRootedDirectoryCreation {
        size_t directories_created;
        int complete;
        UmiStatus status;
    } UmiRootedDirectoryCreation;

    /** Ensure a portable relative directory chain beneath an existing absolute
     * root. Existing ordinary directories are accepted; links/reparse points
     * and nondirectory components are refused. Missing descendants are created
     * one component at a time under checked parents. The root is never created.
     * POSIX uses descriptor-relative operations; Windows holds checked parents
     * without delete sharing. Ancestors above the root remain trusted.
     *
     * report is required and reset on every attempt. directories_created counts
     * successful native creates, including partial progress on later failure.
     * No directory is removed or rolled back. New POSIX directories request
     * mode 0700 subject to umask/default ACLs; Windows inherits parent policy.
     * No durability, workspace trust, concurrent-content lock or subsequent-call
     * reservation is granted. Serialize the caller's edits. Report storage must
     * not overlap input strings, which remain readable for the call. */
    UmiStatus UmiRootedDirectoryEnsure(const char *root, const char *relativePath,
        UmiRootedDirectoryCreation *report);


    /** Create a new final directory with an exclusive native operation.
     * Missing intermediate descendants are prepared like Ensure. An existing
     * final name returns ALREADY_EXISTS even when it is an ordinary directory.
     * Partial parent creation is reported and retained. No reservation survives
     * this call; callers must still prevent external destination mutation. */
    UmiStatus UmiRootedDirectoryCreate(const char *root, const char *relativePath,
        UmiRootedDirectoryCreation *report);

    /* Read a regular file into owned size+1 storage under maximumBytes. Binary
 * bytes are retained and a trailing NUL is added. Free with UmiRootedFileFree.
 * Failure clears both outputs and never publishes a partial read. */
    UmiStatus UmiRootedFileRead(const char *root, const char *relativePath, size_t maximumBytes,
                                unsigned char **outBytes, size_t *outSize);
    void UmiRootedFileFree(void *bytes);

    /* Stage a complete sibling file, flush/close it, then replace the selected
 * regular file or create an absent leaf. Failure before publication preserves
 * the destination. This is one-file publication, not a multi-file transaction.
 * No parent is created. A link, directory, device or read-only Windows leaf is
 * refused. POSIX permission bits are copied from an existing regular file;
 * ownership, ACLs, extra streams and other metadata are not copied. Hard-linked
 * aliases retain the previous inode. A failed cleanup may leave a staging file.
 * Power-loss durability of the containing directory is not guaranteed. */
    UmiStatus UmiRootedFileWrite(const char *root, const char *relativePath, const void *bytes, size_t size);

    /* Exclusive new-file creation is separate from replacement writes. The
 * caller supplies an existing root and existing relative parents. Existing
 * leaves (including links) are never opened or overwritten. No directory is
 * created and no file is deleted. POSIX files start with mode 0600, subject
 * to the process umask; Windows uses the parent's inherited access policy.
 *
 * report is required and is initialized on every call. created becomes true
 * immediately after the native exclusive open succeeds, even if inspection,
 * writing, flushing or closing later fails. bytes_written counts accepted
 * bytes; complete is true only when writing, flushing and closing succeed.
 * On failure with created=true, retain and inspect the partial file before
 * retrying with a new name. A successful close is not a power-loss guarantee.
 * This operation does not hide an incomplete new file from other readers on
 * every platform. It grants no project execution trust. Input must remain
 * valid for the call and must not alias the report. */
    typedef struct UmiRootedFileCreation {
        size_t bytes_written;
        int created;
        int complete;
        UmiStatus status;
    } UmiRootedFileCreation;
    UmiStatus UmiRootedFileCreate(const char *root, const char *relativePath,
        const void *bytes, size_t size, UmiRootedFileCreation *report);

    /* Remove one regular leaf. A missing file returns NOT_FOUND; links and
 * directories are refused. Existence returns OK/false only for a missing leaf
 * or parent, never for denied access or an invalid path. */
    UmiStatus UmiRootedFileRemove(const char *root, const char *relativePath);
    UmiStatus UmiRootedFileExists(const char *root, const char *relativePath, int *outExists);
#ifdef __cplusplus
}
#endif
#endif
