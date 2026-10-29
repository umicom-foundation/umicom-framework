/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/library_choices_fixture.h
 *
 * PURPOSE:
 *   Provide a small independent learning manifest for remembered-library regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_CHOICES_FIXTURE_H
#define UMICOM_LEARNING_CHOICES_FIXTURE_H
#include "umicom/teacher/learning_library_choices.h"
#include "umicom/platform/secure_random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus ChoiceFixturePack(const char *root, const char *id, const char *product,
    const char *locale, uint64_t revision, int write_files, UmiLearningPack **out)
{
    static const char lesson[] = "Read the admitted library resource.";
    char digest[65], manifest[4096];
    UmiStatus status = UmiSha256Buffer(lesson, sizeof(lesson) - 1U, digest);
    if (status != UMI_STATUS_OK) return status;
    int n = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Learning choices\",\"product\":\"%s\",\"locale\":\"%s\",\"revision\":%llu,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,9]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Choose a library\",\"topic\":\"libraries\",\"kind\":\"guide\","
        "\"path\":\"guide.txt\",\"sha256\":\"%s\"}]}", id, product, locale,
        (unsigned long long)revision, digest);
    if (n < 0 || (size_t)n >= sizeof(manifest)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (write_files) status = UmiRootedFileWrite(root, "guide.txt", lesson, sizeof(lesson) - 1U);
    if (status == UMI_STATUS_OK && write_files)
        status = UmiRootedFileWrite(root, "learning-pack.json", manifest, (size_t)n);
    return status == UMI_STATUS_OK ? UmiLearningPackCreate(manifest, (size_t)n, root,
        (UmiVersion){0,9,0}, out) : status;
}
#endif
