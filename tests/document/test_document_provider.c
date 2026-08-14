/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_provider.c
 * PURPOSE: Verify the local provider ABI, atomic write, read and stat operations.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

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
    (void)printf("document provider passed\n");
    return 0;
}
