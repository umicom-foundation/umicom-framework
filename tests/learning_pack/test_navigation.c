/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_navigation.c
 *
 * PURPOSE:
 *   Verify that local learning links cannot escape or ambiguously select content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_navigation.h"
#include <stdio.h>
#include <string.h>

#define HASH "0000000000000000000000000000000000000000000000000000000000000000"
#define RESOURCE(ID, PATH, KIND) "{\"id\":\"" ID "\",\"title\":\"" ID \
    "\",\"topic\":\"navigation\",\"kind\":\"" KIND "\",\"path\":\"" PATH "\",\"sha256\":\"" HASH "\"}"
static const char head[] = "{\"schema\":\"umicom.learning.pack\",\"version\":1,"
    "\"id\":\"framework.navigation\",\"title\":\"Learning navigation\",\"product\":\"framework\","
    "\"locale\":\"en\",\"revision\":1,\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},"
    "\"resources\":["
    RESOURCE("intro", "chapters/intro.html", "lesson") ","
    RESOURCE("next", "chapters/next.html", "lesson") ","
    RESOURCE("diagram", "chapters/images/diagram.svg", "image") ","
    RESOURCE("sample", "chapters/examples/sample.c", "example");
typedef struct Case {
    const char *name, *current, *href, *id, *fragment;
    UmiStatus expected;
    int path_only;
} Case;
static const Case cases[] = {
    {"sibling", "intro", "next.html", "next", "", UMI_STATUS_OK, 0},
    {"nested-image", "intro", "images/diagram.svg", "diagram", "", UMI_STATUS_OK, 0},
    {"nested-example", "intro", "examples/sample.c", "sample", "", UMI_STATUS_OK, 0},
    {"fragment", "intro", "next.html#read-code", "next", "read-code", UMI_STATUS_OK, 0},
    {"same-document", "intro", "#step_1:code", "intro", "step_1:code", UMI_STATUS_OK, 0},
    {"empty-link", "intro", "", "intro", "", UMI_STATUS_OK, 0},
    {"empty-fragment", "intro", "#", "intro", "", UMI_STATUS_OK, 0},
    {"unknown-current", "missing", "next.html", "", "", UMI_STATUS_NOT_FOUND, 0},
    {"undeclared", "intro", "private.html", "", "", UMI_STATUS_NOT_FOUND, 0},
    {"parent", "intro", "../intro.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"dot", "intro", "./next.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"absolute", "intro", "/chapters/next.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"network", "intro", "//server/next.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"drive", "intro", "C:/next.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"https", "intro", "https://example.invalid/", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"script", "intro", "javascript:void(0)", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"encoded", "intro", "%2e%2e/next.html", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"query", "intro", "next.html?user=example", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"backslash", "intro", "images\\diagram.svg", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"control", "intro", "next.html\n", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"anchor-space", "intro", "#two words", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"anchor-escape", "intro", "#%31", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"two-fragments", "intro", "#one#two", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"git-directory", "intro", ".git/config", "", "", UMI_STATUS_PERMISSION_DENIED, 0},
    {"device", "intro", "CON", "", "", UMI_STATUS_INVALID_ARGUMENT, 0},
    {"path", "", "chapters/next.html", "next", "", UMI_STATUS_OK, 1},
    {"path-slashes", "", "chapters\\next.html", "next", "", UMI_STATUS_OK, 1},
    {"ambiguous", "intro", "next.html", "", "", UMI_STATUS_ALREADY_EXISTS, 0},
    {"ambiguous-path", "", "chapters/next.html", "", "", UMI_STATUS_ALREADY_EXISTS, 1}
};
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int result = 0;
    UmiLearningPack *pack = NULL;
    char json[4096];
    const int duplicate = strcmp(argv[1], "ambiguous") == 0 || strcmp(argv[1], "ambiguous-path") == 0;
    int size = snprintf(json, sizeof(json), "%s%s]}", head,
        duplicate ? "," RESOURCE("other", "chapters\\\\next.html", "guide") : "");
    CHECK(size > 0 && (size_t)size < sizeof(json));
    UmiStatus status = UmiLearningPackCreate(json, (size_t)size, argv[2],
        (UmiVersion){0U, 9U, 0U}, &pack);
    CHECK(status == UMI_STATUS_OK);
    UmiLearningPackLink selected, before;
    memset(&selected, 0x5a, sizeof(selected));
    memcpy(&before, &selected, sizeof(before));
    if (strcmp(argv[1], "long-anchor") == 0 || strcmp(argv[1], "long-link") == 0) {
        char long_text[UMI_TEACHER_PATH_CAPACITY + UMI_LEARNING_FRAGMENT_CAPACITY + 1U];
        memset(long_text, 'a', sizeof(long_text) - 1U);
        long_text[sizeof(long_text) - 1U] = '\0';
        if (strcmp(argv[1], "long-anchor") == 0) {
            long_text[0] = '#';
            long_text[UMI_LEARNING_FRAGMENT_CAPACITY + 1U] = '\0';
        }
        status = UmiLearningPackResolveLink(pack, "intro", long_text, &selected);
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&selected, &before, sizeof(selected)) == 0);
        goto cleanup;
    }
    if (strcmp(argv[1], "invalid-arguments") == 0) {
        CHECK(UmiLearningPackResolveLink(NULL, "intro", "next.html", &selected) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackResolveLink(pack, NULL, "next.html", &selected) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackResolveLink(pack, "intro", NULL, &selected) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackResolveLink(pack, "intro", "next.html", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningPackFindPath(pack, NULL, &selected.resource) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&selected, &before, sizeof(selected)) == 0);
        goto cleanup;
    }
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const Case *scenario = &cases[i];
        if (strcmp(argv[1], scenario->name) != 0) continue;
        status = scenario->path_only
            ? UmiLearningPackFindPath(pack, scenario->href, &selected.resource)
            : UmiLearningPackResolveLink(pack, scenario->current, scenario->href, &selected);
        CHECK(status == scenario->expected);
        if (status == UMI_STATUS_OK) {
            CHECK(strcmp(selected.resource.id, scenario->id) == 0);
            if (!scenario->path_only) CHECK(strcmp(selected.fragment, scenario->fragment) == 0);
            /* Metadata is copied and remains readable after the pack closes. */
            UmiLearningPackDestroy(pack);
            pack = NULL;
            CHECK(strcmp(selected.resource.id, scenario->id) == 0);
        } else CHECK(memcmp(&selected, &before, sizeof(selected)) == 0);
        goto cleanup;
    }
    CHECK(!"Unknown learning navigation case");
cleanup:
    UmiLearningPackDestroy(pack);
    return result;
}
