/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/path_uri.h
 *
 * PURPOSE:
 *   Encode UTF-8 native paths as URI references without an editor or UI dependency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_PATH_URI_H
#define UMICOM_PLATFORM_PATH_URI_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Both functions use Platform's host-native lexical path normalization, not
 * filesystem resolution. Inputs are terminated UTF-8. Outputs are complete
 * NUL-terminated strings and remain unchanged on any failure; input/output may
 * alias. Capacity includes NUL. Paths must fit UMI_PATH_CAPACITY after copying.
 * This encodes syntax only; it does not establish file existence or permission.
 * FileUri preserves the Document API's existing drive/UNC spelling. */
UmiStatus UmiFileUriFromPath(const char *path, char *out_uri, size_t capacity);
/* Absolute paths become file URIs. Ordinary relative paths remain relative;
 * reserved characters, including colons, are percent-encoded. No cwd is read.
 * Windows drive-relative and root-relative names are rejected because their
 * meaning depends on a process/drive context. Empty names are invalid. */
UmiStatus UmiPathUriReference(const char *path, char *out_uri, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
