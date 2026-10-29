/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_help.c
 *
 * PURPOSE:
 *   Verify exact help routing, bounded metadata and independent result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
#define HASH "0000000000000000000000000000000000000000000000000000000000000000"
static UmiStatus Pack(const char *root, unsigned revision, const char *locale, UmiLearningPack **out)
{
    char json[3000];
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"framework.guides\","
        "\"title\":\"Shared guides\",\"product\":\"framework\",\"locale\":\"%s\",\"revision\":%u,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"saving\",\"title\":\"Save a document\",\"topic\":\"files\",\"kind\":\"guide\","
        "\"path\":\"saving.html\",\"sha256\":\"" HASH "\"},"
        "{\"id\":\"lesson\",\"title\":\"Read files\",\"topic\":\"files\",\"kind\":\"lesson\","
        "\"path\":\"lesson.html\",\"sha256\":\"" HASH "\"},"
        "{\"id\":\"example\",\"title\":\"Source example\",\"topic\":\"files\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"" HASH "\"}]}", locale, revision);
    if (n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiLearningPackCreate(json, (size_t)n, root, (UmiVersion){0U,9U,0U}, out);
}
static UmiLearningHelpRoute Route(void)
{
    UmiLearningHelpRoute route = {0};
    strcpy(route.request.product, "studio");
    strcpy(route.request.locale, "en-GB");
    route.request.kind = UMI_LEARNING_HELP_COMMAND;
    strcpy(route.request.key, "document.save");
    strcpy(route.pack_id, "framework.guides");
    strcpy(route.resource_id, "saving");
    return route;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int result = 0;
    const char *mode = argv[1];
    UmiLearningHelpMap *map = NULL;
    UmiLearningShelf *shelf = NULL;
    UmiLearningPack *pack = NULL;
    UmiLearningHelpRoute routes[2] = {Route(), Route()}, copied;
    UmiLearningHelpRequest request = routes[0].request;
    UmiLearningHelpSelection selection, guard;
    memset(&selection, 0xa5, sizeof(selection));
    memcpy(&guard, &selection, sizeof(guard));
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    CHECK(Pack(argv[2], 2U, strcmp(mode, "target-locale") == 0 ? "de" : "en-GB", &pack) == UMI_STATUS_OK);
    CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
    if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiLearningHelpMapCreate(NULL, 1U, &map) == UMI_STATUS_INVALID_ARGUMENT && map == NULL);
        CHECK(UmiLearningHelpMapCreate(routes, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpMapCreate(routes, UMI_LEARNING_HELP_MAX_ROUTES + 1U, &map) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiLearningHelpMapCount(NULL) == 0U);
        CHECK(UmiLearningHelpMapRouteAt(NULL, 0U, &copied) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningHelpResolve(NULL, shelf, &request, &selection) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&selection, &guard, sizeof(guard)) == 0);
        UmiLearningHelpMapDestroy(NULL);
        goto cleanup;
    }
    if (strcmp(mode, "duplicate") == 0 || strcmp(mode, "duplicate-locale") == 0) {
        if (strcmp(mode, "duplicate-locale") == 0) strcpy(routes[1].request.locale, "EN-gb");
        strcpy(routes[1].resource_id, "lesson");
        CHECK(UmiLearningHelpMapCreate(routes, 2U, &map) == UMI_STATUS_ALREADY_EXISTS && map == NULL);
        goto cleanup;
    }
    if (strcmp(mode, "unterminated") == 0) {
        for (unsigned field = 0U; field < 5U; ++field) {
            routes[0] = Route();
            if (field == 0U) memset(routes[0].request.product, 'p', sizeof(routes[0].request.product));
            if (field == 1U) memset(routes[0].request.locale, 'l', sizeof(routes[0].request.locale));
            if (field == 2U) memset(routes[0].request.key, 'k', sizeof(routes[0].request.key));
            if (field == 3U) memset(routes[0].pack_id, 'p', sizeof(routes[0].pack_id));
            if (field == 4U) memset(routes[0].resource_id, 'r', sizeof(routes[0].resource_id));
            CHECK(UmiLearningHelpMapCreate(routes, 1U, &map) == UMI_STATUS_CAPACITY_EXCEEDED && map == NULL);
        }
        goto cleanup;
    }
    if (strcmp(mode, "invalid-identities") == 0) {
        const char *bad[] = {"", ".save", "document/save", "https://host", "file name", "bad\nkey", "caf\xc3\xa9"};
        for (size_t i = 0U; i < sizeof(bad)/sizeof(bad[0]); ++i) {
            routes[0] = Route(); strcpy(routes[0].request.key, bad[i]);
            CHECK(UmiLearningHelpMapCreate(routes, 1U, &map) == UMI_STATUS_INVALID_ARGUMENT && map == NULL);
        }
        const char *locales[] = {"", "-en", "en-", "en--GB", "en_GB", "en GB"};
        for (size_t i = 0U; i < sizeof(locales)/sizeof(locales[0]); ++i) {
            routes[0] = Route(); strcpy(routes[0].request.locale, locales[i]);
            CHECK(UmiLearningHelpMapCreate(routes, 1U, &map) == UMI_STATUS_INVALID_ARGUMENT && map == NULL);
        }
        routes[0] = Route(); routes[0].request.kind = (UmiLearningHelpKind)0;
        CHECK(UmiLearningHelpMapCreate(routes, 1U, &map) == UMI_STATUS_INVALID_ARGUMENT);
        routes[0].request.kind = (UmiLearningHelpKind)4;
        CHECK(UmiLearningHelpMapCreate(routes, 1U, &map) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "capacity") == 0) {
        UmiLearningHelpRoute *many = calloc(UMI_LEARNING_HELP_MAX_ROUTES, sizeof(*many));
        CHECK(many != NULL);
        for (size_t i = 0U; i < UMI_LEARNING_HELP_MAX_ROUTES; ++i) {
            many[i] = Route();
            (void)snprintf(many[i].request.key, sizeof(many[i].request.key), "command.%zu", i);
        }
        UmiStatus status = UmiLearningHelpMapCreate(many, UMI_LEARNING_HELP_MAX_ROUTES, &map);
        free(many);
        CHECK(status == UMI_STATUS_OK && UmiLearningHelpMapCount(map) == UMI_LEARNING_HELP_MAX_ROUTES);
        CHECK(UmiLearningHelpMapRouteAt(map, UMI_LEARNING_HELP_MAX_ROUTES - 1U, &copied) == UMI_STATUS_OK);
        CHECK(UmiLearningHelpResolve(map, shelf, &copied.request, &selection) == UMI_STATUS_OK);
        goto cleanup;
    }
    size_t count = 1U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "empty") == 0) { count = 0U; expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(mode, "diagnostic") == 0) {
        request.kind = routes[0].request.kind = UMI_LEARNING_HELP_DIAGNOSTIC;
        strcpy(request.key, "UMI_STATUS_IO_ERROR"); strcpy(routes[0].request.key, request.key);
    } else if (strcmp(mode, "topic") == 0) {
        request.kind = routes[0].request.kind = UMI_LEARNING_HELP_TOPIC;
        strcpy(routes[0].resource_id, "lesson");
    } else if (strcmp(mode, "kind-separation") == 0) {
        routes[1].request.kind = request.kind = UMI_LEARNING_HELP_DIAGNOSTIC;
        strcpy(routes[1].resource_id, "lesson"); count = 2U;
    } else if (strcmp(mode, "product-case") == 0) {
        strcpy(request.product, "Studio"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "key-case") == 0) {
        strcpy(request.key, "Document.Save"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "locale-case") == 0) strcpy(request.locale, "EN-gb");
    else if (strcmp(mode, "locale-no-fallback") == 0) {
        strcpy(request.locale, "en"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "target-locale") == 0) expected = UMI_STATUS_UNAVAILABLE;
    else if (strcmp(mode, "missing-route") == 0) {
        strcpy(request.key, "document.open"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "missing-pack") == 0) {
        strcpy(routes[0].pack_id, "unloaded"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "missing-resource") == 0) {
        strcpy(routes[0].resource_id, "absent"); expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "wrong-kind") == 0) {
        strcpy(routes[0].resource_id, "example"); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "request-invalid") == 0) {
        memset(request.key, 'k', sizeof(request.key)); expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else CHECK(strcmp(mode, "command") == 0 || strcmp(mode, "outputs") == 0 ||
        strcmp(mode, "snapshot") == 0 || strcmp(mode, "removed-pack") == 0 || strcmp(mode, "replacement") == 0);
    CHECK(UmiLearningHelpMapCreate(count == 0U ? NULL : routes, count, &map) == UMI_STATUS_OK);
    CHECK(UmiLearningHelpMapCount(map) == count);
    /* Releasing or changing caller input cannot change the published policy. */
    memset(routes, 0, sizeof(routes));
    if (strcmp(mode, "removed-pack") == 0) {
        CHECK(UmiLearningShelfRemove(shelf, "framework.guides") == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    CHECK(UmiLearningHelpResolve(map, shelf, &request, &selection) == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(&selection, &guard, sizeof(guard)) == 0);
    else {
        CHECK(strcmp(selection.route.request.product, "studio") == 0);
        CHECK(strcmp(selection.pack.product, "framework") == 0 && selection.pack.revision == 2U);
        CHECK(strcmp(selection.resource.id, strcmp(mode,"topic") == 0 ||
            strcmp(mode,"kind-separation") == 0 ? "lesson" : "saving") == 0);
        if (strcmp(mode, "snapshot") == 0) {
            UmiLearningShelfDestroy(shelf); shelf = NULL;
            UmiLearningHelpMapDestroy(map); map = NULL;
            CHECK(strcmp(selection.pack.id, "framework.guides") == 0);
            CHECK(strcmp(selection.resource.sha256, HASH) == 0);
        }
        if (strcmp(mode, "replacement") == 0) {
            CHECK(UmiLearningShelfRemove(shelf, "framework.guides") == UMI_STATUS_OK);
            CHECK(Pack(argv[2], 3U, "en-GB", &pack) == UMI_STATUS_OK);
            CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
            UmiLearningHelpSelection current;
            CHECK(UmiLearningHelpResolve(map, shelf, &request, &current) == UMI_STATUS_OK);
            CHECK(current.pack.revision == 3U && selection.pack.revision == 2U);
        }
        if (strcmp(mode, "outputs") == 0) {
            memcpy(&guard, &selection, sizeof(guard));
            CHECK(UmiLearningHelpResolve(map, NULL, &request, &selection) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningHelpResolve(map, shelf, NULL, &selection) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningHelpResolve(map, shelf, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&selection, &guard, sizeof(guard)) == 0);
            memset(&copied, 0x5a, sizeof(copied));
            UmiLearningHelpRoute route_guard;
            memcpy(&route_guard, &copied, sizeof(copied));
            CHECK(UmiLearningHelpMapRouteAt(map, count, &copied) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&copied, &route_guard, sizeof(copied)) == 0);
            CHECK(UmiLearningHelpMapRouteAt(map, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        }
    }
cleanup:
    UmiLearningPackDestroy(pack);
    UmiLearningShelfDestroy(shelf);
    UmiLearningHelpMapDestroy(map);
    return result;
}
