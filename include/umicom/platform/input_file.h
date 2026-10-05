/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/input_file.h
 * PURPOSE: Read an explicitly chosen regular local file under a caller-supplied memory limit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_INPUT_FILE_H
#define UMICOM_PLATFORM_INPUT_FILE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Require the same ordinary absolute path syntax as UmiOutputFileValidatePath.
 * No working-directory lookup, directory creation or file write occurs. The
 * final path component must be a regular file, not a link or device. Parent
 * aliases may be followed: choose a trusted directory; this is not confinement.
 * Reject a file larger than maximumBytes before allocation, and check the same
 * open handle for observable changes after reading. This is not a guarantee
 * against a writer that changes data without observable metadata differences.
 * The result owns size+1 bytes with a trailing NUL; embedded NULs remain valid
 * binary data. Empty files succeed. Failure clears both output arguments.
 * I/O may block: call from a worker when used by an interactive application. */
    UmiStatus UmiInputFileRead(const char *path, size_t maximumBytes, unsigned char **outBytes,
                               size_t *outSize);
    void UmiInputFileFree(void *bytes);
#ifdef __cplusplus
}
#endif
#endif
