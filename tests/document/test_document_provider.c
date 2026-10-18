/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_provider.c
 * PURPOSE: Verify the local provider ABI, atomic write, read and stat operations.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
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
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiDocumentFileInfo info;
    assert(umi_document_provider_validate(&provider) == UMI_STATUS_OK);
    assert(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), root, "umicom-b39-provider.txt") == UMI_STATUS_OK);
    assert(umi_document_provider_write(&provider, path, "provider", 8U, 1) == UMI_STATUS_OK);
    assert(umi_document_provider_read(&provider, path, &bytes, &size) == UMI_STATUS_OK);
    assert(size == 8U && memcmp(bytes, "provider", 8U) == 0);
    umi_document_provider_release_bytes(&provider, bytes);
    assert(umi_document_provider_stat(&provider, path, &info) == UMI_STATUS_OK);
    assert(info.exists && info.regular_file && info.byte_count == 8U);
    assert(provider.remove(provider.instance, path) == UMI_STATUS_OK);
    /* Use the same UTF-8 identity across write/read/stat/remove on Windows.
     * Include a supplementary character to exercise a UTF-16 surrogate pair. */
    assert(umi_fs_join(path, sizeof(path), root, "umicom-provider-caf\xc3\xa9-\xf0\x9f\x8c\x90.txt") == UMI_STATUS_OK);
    assert(umi_document_provider_write(&provider, path, "unicode", 7U, 1) == UMI_STATUS_OK);
    assert(umi_document_provider_stat(&provider, path, &info) == UMI_STATUS_OK);
    assert(info.exists && info.regular_file && info.byte_count == 7U && info.readable && info.writable);
    assert(umi_path_equal(path, info.path));
    bytes = NULL;
    assert(umi_document_provider_read(&provider, path, &bytes, &size) == UMI_STATUS_OK);
    assert(size == 7U && memcmp(bytes, "unicode", 7U) == 0);
    umi_document_provider_release_bytes(&provider, bytes);
    assert(provider.remove(provider.instance, path) == UMI_STATUS_OK);
    assert(umi_document_provider_stat(&provider, path, &info) == UMI_STATUS_NOT_FOUND);
    (void)printf("document provider passed\n");
    return 0;
}
