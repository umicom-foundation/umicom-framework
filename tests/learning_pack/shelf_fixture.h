/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/shelf_fixture.h
 *
 * PURPOSE:
 *   Prepare immutable, versioned packs for catalogue ownership regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LEARNING_SHELF_FIXTURE_H
#define UMICOM_LEARNING_SHELF_FIXTURE_H
#include "umicom/teacher/learning_reader.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static UmiStatus ShelfFixturePack(const char *root, const char *id,
    const char *product, const char *locale, uint64_t revision, UmiLearningPack **out)
{
    char json[2048], hash[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer("lesson", 6U, hash);
    if (status != UMI_STATUS_OK) return status;
    int length = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Shelf lesson\",\"product\":\"%s\",\"locale\":\"%s\","
        "\"revision\":%" PRIu64 ",\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},"
        "\"resources\":[{\"id\":\"lesson\",\"title\":\"Lesson\",\"topic\":\"reading\","
        "\"kind\":\"lesson\",\"path\":\"lesson.txt\",\"sha256\":\"%s\"}]}",
        id, product, locale, revision, hash);
    if (length < 0 || (size_t)length >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiLearningPackCreate(json, (size_t)length, root, (UmiVersion){0U, 9U, 0U}, out);
}
#endif
