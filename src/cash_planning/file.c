/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/file.c
 * PURPOSE: Keep plan and report file access explicit, bounded and non-overwriting.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cash_planning/document.h"
#include "umicom/platform/input_file.h"
#include <string.h>
UmiStatus UmiCashPlanLoad(const char *path, UmiCashPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, UMI_CASH_PLAN_DOCUMENT_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanDecode(bytes, size, out);
    UmiInputFileFree(bytes);
    return status;
}
/* Serialize before opening the destination. An invalid plan or overflowing
 * report therefore creates no file. An actual write failure retains its partial
 * file and receipt, allowing the user to inspect it before choosing a new path. */
static UmiStatus Save(const char *path, const UmiCashPlan *plan, int csv, UmiOutputFileSnapshot *receipt)
{
    if (receipt != NULL)
        memset(receipt, 0, sizeof *receipt);
    char *bytes = NULL;
    size_t size = 0U;
    UmiOutputFile *file = NULL;
    UmiStatus status = csv ? UmiCashPlanCsv(plan, &bytes, &size) : UmiCashPlanEncode(plan, &bytes, &size);
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
    UmiCashPlanFreeBytes(bytes);
    return status;
}
UmiStatus UmiCashPlanSaveNew(const char *path, const UmiCashPlan *plan, UmiOutputFileSnapshot *receipt)
{
    return Save(path, plan, 0, receipt);
}
UmiStatus UmiCashPlanExportNew(const char *path, const UmiCashPlan *plan, UmiOutputFileSnapshot *receipt)
{
    return Save(path, plan, 1, receipt);
}
