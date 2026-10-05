/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/setup_document.h
 * PURPOSE: Read and write explicit local debug setup documents without execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SETUP_DOCUMENT_H
#define UMICOM_DEBUG_SETUP_DOCUMENT_H
#include "umicom/debug/setup.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DEBUG_SETUP_DOCUMENT_LIMIT (2U * 1024U * 1024U)
    /* Encode creates an owned, NUL-terminated JSON string; size excludes NUL.
 * Decode accepts exactly size bytes and rejects unknown/duplicate/missing
 * fields, invalid UTF-8, unsupported formats and oversized collections.
 * Failure clears outputs. FreeBytes releases only Encode's allocated bytes.
 * JSON contains source locations and expressions: save it in a private place
 * if those reveal confidential project information. It contains no run command. */
    UmiStatus UmiDebugSetupEncode(const UmiDebugSetup *setup, char **out, size_t *outSize);
    void UmiDebugSetupFreeBytes(char *bytes);
    UmiStatus UmiDebugSetupDecode(const void *bytes, size_t size, UmiDebugSetup **out);
    /* Explicit file actions, intended for worker threads. Load requires a regular
 * file with an absolute path and does not change any live settings. SaveNew
 * never overwrites a leaf; its trusted parent must already exist. Failed writes
 * can leave a partial new file, identified by the optional receipt. Do not retry
 * over that file. Loading expressions does not make them trusted to evaluate. */
    UmiStatus UmiDebugSetupLoad(const char *path, UmiDebugSetup **out);
    UmiStatus UmiDebugSetupSaveNew(const char *path, const UmiDebugSetup *setup,
                                   UmiOutputFileSnapshot *receipt);
#ifdef __cplusplus
}
#endif
#endif
