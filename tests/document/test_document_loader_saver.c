/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_loader_saver.c
 * PURPOSE: Verify encoding-aware load and atomic save round trips.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDocumentProvider provider = umi_document_local_provider();
    UmiDocumentSaveOptions save = umi_document_save_options_default();
    UmiDocumentSaveResult saved;
    UmiDocumentLoadOptions load = umi_document_load_options_default();
    UmiDocumentLoadResult loaded;
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    const char text[] = "alpha\nbeta\n";
    assert(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), root, "umicom-b39-loader.txt") == UMI_STATUS_OK);
    save.encoding = UMI_DOCUMENT_ENCODING_UTF16_LE;
    save.include_bom = 1;
    save.line_ending = UMI_DOCUMENT_LINE_ENDING_CRLF;
    assert(umi_document_saver_write(&provider, path, text, strlen(text),
                                    &save, &saved) == UMI_STATUS_OK);
    load.normalise_to = UMI_DOCUMENT_LINE_ENDING_LF;
    assert(umi_document_load(&provider, path, &load, &loaded) == UMI_STATUS_OK);
    assert(strcmp(loaded.text, text) == 0);
    assert(loaded.detected_encoding == UMI_DOCUMENT_ENCODING_UTF16_LE);
    assert(loaded.detected_line_ending == UMI_DOCUMENT_LINE_ENDING_CRLF);
    umi_document_load_result_dispose(&loaded);
    assert(remove(path) == 0);
    (void)printf("document loader/saver passed\n");
    return 0;
}
