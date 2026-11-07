/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/authoring_fixture.h
 *
 * PURPOSE:
 *   Create isolated learning-pack resources shared by service and native command regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LEARNING_AUTHORING_FIXTURE_H
#define UMICOM_TEST_LEARNING_AUTHORING_FIXTURE_H
#include "umicom/teacher/learning_pack_authoring.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>
static UmiStatus AuthoringFixture(const char *root)
{
    static const char *const ids[] = {"lesson", "guide", "example", "image"};
    static const char *const paths[] = {"lesson.md", "guide.html", "example.c", "image.svg"};
    static const char bytes[] = "original\n";
    char digest[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer(bytes, sizeof(bytes) - 1U, digest);
    if (status != UMI_STATUS_OK) return status;
    char json[4096];
    const char *prefix = "{\"schema\":\"umicom.learning.pack\",\"version\":1,"
        "\"id\":\"test.authoring\",\"title\":\"A \\\"quote\\\" and \\\\ path \\u03a9\","
        "\"product\":\"studio\",\"locale\":\"en-GB\",\"revision\":2,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[1,2,3]},\"resources\":[";
    size_t used = strlen(prefix);
    (void)memcpy(json, prefix, used + 1U);
    for (size_t i = 0U; i < 4U; ++i) {
        status = UmiRootedFileWrite(root, paths[i], bytes, sizeof(bytes) - 1U);
        if (status != UMI_STATUS_OK) return status;
        int n = snprintf(json + used, sizeof(json) - used,
            "%s{\"id\":\"%s\",\"title\":\"Resource %zu\",\"topic\":\"ownership\","
            "\"kind\":\"%s\",\"path\":\"%s\",\"sha256\":\"%s\"%s}",
            i == 0U ? "" : ",", ids[i], i, ids[i], paths[i], digest,
            i == 0U ? ",\"example\":\"example\"" : "");
        if (n < 0 || (size_t)n >= sizeof(json) - used) return UMI_STATUS_CAPACITY_EXCEEDED;
        used += (size_t)n;
    }
    if (sizeof(json) - used < 3U) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(json + used, "]}", 3U);
    return UmiRootedFileWrite(root, "learning-pack.json", json, used + 2U);
}
#endif
