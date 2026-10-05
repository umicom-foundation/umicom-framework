/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/rooted_files.h
 * PURPOSE: Provide bounded file access beneath an explicitly chosen local workspace root.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
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

    /* Remove one regular leaf. A missing file returns NOT_FOUND; links and
 * directories are refused. Existence returns OK/false only for a missing leaf
 * or parent, never for denied access or an invalid path. */
    UmiStatus UmiRootedFileRemove(const char *root, const char *relativePath);
    UmiStatus UmiRootedFileExists(const char *root, const char *relativePath, int *outExists);
#ifdef __cplusplus
}
#endif
#endif
