/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_document_link_closure.c
 *
 * PURPOSE:
 *   Regression-test the static-library dependency that caused the Windows
 *   MinGW Studio link failure: Studio Runtime lives in Umicom::developer and
 *   projects UmiDocumentTextEncoding, therefore linking Umicom::developer alone
 *   must bring Umicom::document and umi_document_encoding_text into the link.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/document/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(
        umi_document_encoding_text(UMI_DOCUMENT_ENCODING_UTF8),
        "UTF-8") == 0);
    assert(strcmp(
        umi_document_encoding_text(UMI_DOCUMENT_ENCODING_UTF16_LE),
        "UTF-16 LE") == 0);
    assert(strcmp(
        umi_document_line_ending_text(UMI_DOCUMENT_LINE_ENDING_CRLF),
        "CRLF") == 0);

    return 0;
}
