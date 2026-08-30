/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_location_link.c
 *
 * PURPOSE:
 *   Verify clickable file:line:column extraction.
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
#include "umicom/developer_productivity/location_link.h"

int main(void)
{
    UmiDeveloperProductivityLocation location;
    int matched = 0;

    assert(umi_developer_location_link_parse(
        "src/main.c:42:7: error",
        &location,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(strcmp(location.uri, "src/main.c") == 0);
    assert(location.line == 42U);
    assert(location.column == 7U);

    matched = 0;
    assert(umi_developer_location_link_parse(
        "C:/work/src/main.c:51:9: warning",
        &location,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(strcmp(location.uri, "C:/work/src/main.c") == 0);
    assert(location.line == 51U);
    return 0;
}
