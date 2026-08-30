/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_line_endings.c
 * PURPOSE: Verify mixed-line analysis and deterministic CRLF/LF conversion.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char input[] = "one\r\ntwo\nthree\rfour";
    UmiDocumentLineEndingAnalysis analysis;
    char *normalised = NULL;
    size_t length = 0U;
    assert(umi_document_line_endings_analyse(input, strlen(input), &analysis) == UMI_STATUS_OK);
    assert(analysis.mixed);
    assert(analysis.lf_count == 1U && analysis.crlf_count == 1U && analysis.cr_count == 1U);
    assert(umi_document_line_endings_normalise(input, strlen(input),
                                               UMI_DOCUMENT_LINE_ENDING_CRLF, 1,
                                               &normalised, &length) == UMI_STATUS_OK);
    assert(strcmp(normalised, "one\r\ntwo\r\nthree\r\nfour\r\n") == 0);
    assert(length == strlen(normalised));
    umi_document_line_endings_free(normalised);
    (void)printf("document line endings passed\n");
    return 0;
}
