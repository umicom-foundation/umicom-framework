/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/local_replace.h
 * PURPOSE: Replace an explicitly selected local file using exclusive native staging and UTF-8 paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_LOCAL_REPLACE_H
#define UMICOM_PLATFORM_LOCAL_REPLACE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
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
#ifdef __cplusplus
}
#endif
#endif
