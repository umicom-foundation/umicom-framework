/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/setup_file.c
 * PURPOSE: Keep debugger file I/O explicit and preserve existing or partial files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug/setup_document.h"
#include "umicom/platform/input_file.h"
#include <string.h>
UmiStatus UmiDebugSetupLoad(const char *path, UmiDebugSetup **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, UMI_DEBUG_SETUP_DOCUMENT_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSetupDecode(bytes, size, out);
    UmiInputFileFree(bytes);
    return status;
}
UmiStatus UmiDebugSetupSaveNew(const char *path, const UmiDebugSetup *setup, UmiOutputFileSnapshot *receipt)
{
    if (receipt != NULL)
        memset(receipt, 0, sizeof *receipt);
    char *bytes = NULL;
    size_t size = 0U;
    UmiOutputFile *file = NULL;
    UmiStatus status = UmiDebugSetupEncode(setup, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileWrite(file, bytes, size);
    if (file != NULL)
    {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK)
            status = closed;
        if (receipt != NULL)
            (void)UmiOutputFileRead(file, receipt);
    }
    else if (receipt != NULL)
        receipt->status = status;
    UmiOutputFileDestroy(file);
    UmiDebugSetupFreeBytes(bytes);
    return status;
}
