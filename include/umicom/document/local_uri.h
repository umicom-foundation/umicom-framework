/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/local_uri.h
 * PURPOSE: Resolve explicitly local file identifiers without treating remote or malformed URIs as paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_LOCAL_URI_H
#define UMICOM_DOCUMENT_LOCAL_URI_H
#include "umicom/document/types.h"
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Convert a file URI with an empty or localhost authority into an absolute
 * native path. Scheme and localhost matching are ASCII case-insensitive.
 * Reject query/fragment suffixes, malformed escapes, encoded separators,
 * decoded NUL/control bytes, invalid UTF-8 and network/device path prefixes.
 * Other schemes and remote authorities return NOT_IMPLEMENTED.
 * URI input is limited to 8192 bytes; native path capacity is UMI_PATH_CAPACITY.
 * This only interprets a name. It does not open files, follow links, confine a
 * path to a project, or detect network mounts behind an ordinary local path.
 * Output changes only on success; input and output may alias. */
    /* Windows targets must also pass the platform ordinary-file syntax check:
 * device aliases, trailing-dot/space names and alternate streams are refused. */
    UmiStatus UmiDocumentLocalFileUriToPath(const char *uri, char *out_path, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
