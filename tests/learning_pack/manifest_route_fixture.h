/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/manifest_route_fixture.h
 *
 * PURPOSE:
 *   Write small independent course manifests for provenance and catalogue regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_MANIFEST_ROUTE_FIXTURE_H
#define UMICOM_TEST_MANIFEST_ROUTE_FIXTURE_H
#include "umicom/teacher/learning_pack.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>
/* All substituted strings below are test-owned literals. This fixture is not
 * a production JSON authoring interface for arbitrary user text. */
static UmiStatus RouteFixture(const char *root, const char *manifest,
    const char *id, unsigned revision)
{
    static const char body[] = "# Course\nRead this local lesson.\n";
    char digest[UMI_SHA256_HEX_CAPACITY], json[2048];
    UmiStatus status = UmiSha256Buffer(body, sizeof(body)-1U, digest);
    if (status != UMI_STATUS_OK) return status;
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Course\",\"product\":\"education\",\"locale\":\"en\","
        "\"revision\":%u,\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,9]},"
        "\"resources\":[{\"id\":\"lesson\",\"title\":\"Lesson\",\"topic\":\"learning\","
        "\"kind\":\"lesson\",\"path\":\"lesson.md\",\"sha256\":\"%s\"}]}",id,revision,digest);
    if(n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiRootedFileWrite(root,"lesson.md",body,sizeof(body)-1U);
    if(status == UMI_STATUS_OK)
        status = UmiRootedFileWrite(root,manifest,json,(size_t)n);
    return status;
}
#endif
