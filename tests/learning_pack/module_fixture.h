/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/module_fixture.h
 *
 * PURPOSE:
 *   Create a small declared module for ownership and native action regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_LEARNING_MODULE_FIXTURE_H
#define UMICOM_TEST_LEARNING_MODULE_FIXTURE_H
#include "umicom/teacher/learning_module.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>
static const char module_text[] = "<h1>Learning module</h1><p>Read one example.</p>";
static UmiStatus ModuleFixture(const char *root)
{
    char hash[UMI_SHA256_HEX_CAPACITY], manifest[1024];
    UmiStatus status = UmiSha256Buffer(module_text, sizeof(module_text) - 1U, hash);
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileWrite(root, "lesson.html", module_text, sizeof(module_text) - 1U);
    if (status != UMI_STATUS_OK) return status;
    int n = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"module.fixture\","
        "\"title\":\"Learning module\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"lesson\",\"title\":\"Read an example\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"lesson.html\",\"sha256\":\"%s\"}]}", hash);
    if (n < 0 || (size_t)n >= sizeof(manifest)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiRootedFileWrite(root, "learning-pack.json", manifest, (size_t)n);
}
#endif
