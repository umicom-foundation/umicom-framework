/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_fingerprint.c
 * PURPOSE: Verify deterministic content and local-file fingerprints.
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
    const char text[] = "fingerprint me";
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    UmiDocumentFingerprint first;
    UmiDocumentFingerprint second;
    assert(umi_document_hash_bytes(text, strlen(text)) ==
           umi_document_hash_bytes_portable(text, strlen(text), UINT64_C(14695981039346656037)));
    assert(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), root, "umicom-b39-fingerprint.txt") == UMI_STATUS_OK);
    assert(umi_fs_write_text(path, text) == UMI_STATUS_OK);
    assert(umi_document_fingerprint_file(path, &first) == UMI_STATUS_OK);
    assert(umi_document_fingerprint_file(path, &second) == UMI_STATUS_OK);
    assert(umi_document_fingerprint_equal(&first, &second));
    assert(remove(path) == 0);
    (void)printf("document fingerprint (%s) passed\n", umi_document_fingerprint_backend());
    return 0;
}
