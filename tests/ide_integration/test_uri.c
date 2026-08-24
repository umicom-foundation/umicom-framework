/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_uri.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration uri.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/uri.h"
int main(void)
{
    char path[256];
    assert(umi_ide_uri_to_local_path(
        "file:///tmp/hello%20world.c", path, sizeof(path)) == UMI_STATUS_OK);
#ifndef _WIN32
    assert(strcmp(path, "/tmp/hello world.c") == 0);
#endif
    assert(umi_ide_uri_to_local_path(
        "https://example.com/a.c", path, sizeof(path)) ==
        UMI_STATUS_NOT_IMPLEMENTED);
    return 0;
}

