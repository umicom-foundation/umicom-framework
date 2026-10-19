/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_archive.c
 * Purpose: Retain closed attempts in explicit exclusive files and verify receipts on reopening.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiQualificationArchiveSave(const UmiReleaseQualification *run, const char *absolutePath,
    UmiQualificationArchiveReceipt *outReceipt)
{
    if (run == NULL || absolutePath == NULL || outReceipt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!run->closed) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiOutputFileValidatePath(absolutePath);
    size_t required = 0U;
    if (status == UMI_STATUS_OK) status = UmiReleaseQualificationFormatReport(run, NULL, 0U, &required);
    char *text = NULL;
    if (status == UMI_STATUS_OK) {
        text = malloc(required);
        if (text == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = UmiReleaseQualificationFormatReport(run, text, required, &required);
    UmiQualificationArchiveReceipt receipt = {.identity = run->identity};
    if (status == UMI_STATUS_OK) {
        receipt.bytes = required - 1U;
        status = UmiSha256Buffer(text, receipt.bytes, receipt.reportSha256);
    }
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(absolutePath, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, text, receipt.bytes);
    if (file != NULL) {
        UmiStatus closeStatus = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closeStatus;
    }
    UmiOutputFileDestroy(file); free(text);
    if (status == UMI_STATUS_OK) *outReceipt = receipt;
    return status;
}
UmiStatus UmiQualificationArchiveLoad(const char *absolutePath, const char *expectedSha256,
    UmiQualificationView **outView, UmiQualificationArchiveReceipt *outReceipt)
{
    if (absolutePath == NULL || outView == NULL || *outView != NULL || outReceipt == NULL ||
        (expectedSha256 != NULL && !UmiQualificationDigestValid(expectedSha256)))
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL; size_t length = 0U;
    UmiStatus status = UmiInputFileRead(absolutePath, UMI_QUALIFICATION_REPORT_LIMIT - 1U, &bytes, &length);
    if (status == UMI_STATUS_OK && expectedSha256 != NULL) {
        char actual[65];
        status = UmiSha256Buffer(bytes, length, actual);
        if (status == UMI_STATUS_OK && strcmp(actual, expectedSha256) != 0) status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK)
        status = UmiQualificationArchiveParse((const char *)bytes, length, outView, outReceipt);
    UmiInputFileFree(bytes); return status;
}
