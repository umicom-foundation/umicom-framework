/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_help.c
 *
 * PURPOSE:
 *   Keep contextual-help policy explicit, bounded and independent of filesystem I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_help.h"
#include <stdlib.h>
#include <string.h>

struct UmiLearningHelpMap {
    size_t count;
    UmiLearningHelpRoute routes[UMI_LEARNING_HELP_MAX_ROUTES];
};
static int HelpAlphaNumeric(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9');
}
static UmiStatus HelpToken(const char *text, size_t capacity, int locale)
{
    if (memchr(text, '\0', capacity) == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!HelpAlphaNumeric((unsigned char)text[0])) return UMI_STATUS_INVALID_ARGUMENT;
    int segment = 1;
    for (size_t i = 1U; text[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (HelpAlphaNumeric(ch)) segment = 1;
        else if (locale && ch == '-' && segment) segment = 0;
        else if (!locale && (ch == '.' || ch == '-' || ch == '_')) continue;
        else return UMI_STATUS_INVALID_ARGUMENT;
    }
    return locale && !segment ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
static unsigned char HelpFold(unsigned char ch)
{
    return ch >= 'A' && ch <= 'Z' ? (unsigned char)(ch + ('a' - 'A')) : ch;
}
static int HelpLocaleEqual(const char *left, const char *right)
{
    for (size_t i = 0U;; ++i) {
        if (HelpFold((unsigned char)left[i]) != HelpFold((unsigned char)right[i])) return 0;
        if (left[i] == '\0') return 1;
    }
}
static UmiStatus HelpRequestValid(const UmiLearningHelpRequest *request)
{
    if (request == NULL || request->kind < UMI_LEARNING_HELP_COMMAND ||
        request->kind > UMI_LEARNING_HELP_TOPIC) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = HelpToken(request->product, sizeof(request->product), 0);
    if (status == UMI_STATUS_OK) status = HelpToken(request->locale, sizeof(request->locale), 1);
    if (status == UMI_STATUS_OK) status = HelpToken(request->key, sizeof(request->key), 0);
    return status;
}
static int HelpRequestEqual(const UmiLearningHelpRequest *left, const UmiLearningHelpRequest *right)
{
    return left->kind == right->kind && strcmp(left->product, right->product) == 0 &&
        strcmp(left->key, right->key) == 0 && HelpLocaleEqual(left->locale, right->locale);
}
UmiStatus UmiLearningHelpMapCreate(const UmiLearningHelpRoute *routes, size_t count,
    UmiLearningHelpMap **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (count > UMI_LEARNING_HELP_MAX_ROUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (routes == NULL && count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Complete validation precedes allocation/publication. No half-built map
     * can silently fall back to another route after an invalid input row. */
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = HelpRequestValid(&routes[i].request);
        if (status == UMI_STATUS_OK) status = HelpToken(routes[i].pack_id, sizeof(routes[i].pack_id), 0);
        if (status == UMI_STATUS_OK) status = HelpToken(routes[i].resource_id, sizeof(routes[i].resource_id), 0);
        if (status != UMI_STATUS_OK) return status;
        for (size_t earlier = 0U; earlier < i; ++earlier)
            if (HelpRequestEqual(&routes[earlier].request, &routes[i].request))
                return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiLearningHelpMap *map = calloc(1U, sizeof(*map));
    if (map == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (count != 0U) memcpy(map->routes, routes, count * sizeof(*routes));
    map->count = count;
    *out = map;
    return UMI_STATUS_OK;
}
void UmiLearningHelpMapDestroy(UmiLearningHelpMap *map) { free(map); }
size_t UmiLearningHelpMapCount(const UmiLearningHelpMap *map) { return map == NULL ? 0U : map->count; }
UmiStatus UmiLearningHelpMapRouteAt(const UmiLearningHelpMap *map, size_t index,
    UmiLearningHelpRoute *out)
{
    if (map == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= map->count) return UMI_STATUS_NOT_FOUND;
    *out = map->routes[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningHelpResolve(const UmiLearningHelpMap *map, const UmiLearningShelf *shelf,
    const UmiLearningHelpRequest *request, UmiLearningHelpSelection *out)
{
    if (map == NULL || shelf == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = HelpRequestValid(request);
    if (status != UMI_STATUS_OK) return status;
    const UmiLearningHelpRoute *route = NULL;
    for (size_t i = 0U; i < map->count; ++i)
        if (HelpRequestEqual(&map->routes[i].request, request)) { route = &map->routes[i]; break; }
    if (route == NULL) return UMI_STATUS_NOT_FOUND;
    UmiLearningHelpSelection selection = {0};
    selection.route = *route;
    int found = 0;
    for (size_t i = 0U; i < UmiLearningShelfCount(shelf); ++i) {
        status = UmiLearningShelfPackAt(shelf, i, &selection.pack);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(selection.pack.id, route->pack_id) == 0) { found = 1; break; }
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    if (!HelpLocaleEqual(selection.pack.locale, request->locale)) return UMI_STATUS_UNAVAILABLE;
    status = UmiLearningShelfFindResource(shelf, route->pack_id, route->resource_id, &selection.resource);
    if (status != UMI_STATUS_OK) return status;
    if (selection.resource.kind != UMI_LEARNING_RESOURCE_GUIDE &&
        selection.resource.kind != UMI_LEARNING_RESOURCE_LESSON) return UMI_STATUS_INVALID_STATE;
    *out = selection;
    return UMI_STATUS_OK;
}
