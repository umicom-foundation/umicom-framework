/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_uri.c
 * PURPOSE: Verify reversible percent-safe absolute file URI conversion.
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
    char current[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char decoded[UMI_PATH_CAPACITY];
    char uri[UMI_DOCUMENT_URI_CAPACITY];
    assert(umi_fs_current_directory(current, sizeof(current)) == UMI_STATUS_OK);
    assert(umi_path_join(current, "A file #1.c", path, sizeof(path)) == UMI_STATUS_OK);
    assert(umi_document_uri_from_path(path, uri, sizeof(uri)) == UMI_STATUS_OK);
    assert(strncmp(uri, "file://", 7U) == 0);
    assert(strstr(uri, "%20") != NULL && strstr(uri, "%23") != NULL);
    assert(umi_document_uri_to_path(uri, decoded, sizeof(decoded)) == UMI_STATUS_OK);
    assert(umi_path_equal(path, decoded));
    (void)printf("document URI passed\n");
    return 0;
}
