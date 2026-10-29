/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/progress_reader_fixture.h
 *
 * PURPOSE:
 *   Create a verified lesson reader shared by storage and fault-injection tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PROGRESS_READER_FIXTURE_H
#define UMICOM_TEST_PROGRESS_READER_FIXTURE_H
#include "umicom/teacher/learning_progress.h"
#include "umicom/platform/rooted_files.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
static UmiStatus Reader(const char *root, uint64_t revision, const char *body,
    const char *kind, const char *pack_id, const char *resource_id, UmiLearningReader **out)
{
    char hash[UMI_SHA256_HEX_CAPACITY], json[1600];
    *out = NULL;
    UmiStatus status = UmiSha256Buffer(body, strlen(body), hash);
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(root, "lesson.html", body, strlen(body));
    if (status != UMI_STATUS_OK) return status;
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Progress fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":%" PRIu64 ","
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"%s\",\"title\":\"Lesson\",\"topic\":\"records\",\"kind\":\"%s\","
        "\"path\":\"lesson.html\",\"sha256\":\"%s\"}]}", pack_id, revision, resource_id, kind, hash);
    if (n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningPack *pack = NULL;
    status = UmiLearningPackCreate(json, (size_t)n, root, (UmiVersion){0U, 9U, 0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreate(&pack, 4096U, out);
    UmiLearningPackDestroy(pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(*out, resource_id);
    if (status != UMI_STATUS_OK) { UmiLearningReaderDestroy(*out); *out = NULL; }
    return status;
}
#endif
