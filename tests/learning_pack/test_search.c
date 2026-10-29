/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_search.c
 *
 * PURPOSE:
 *   Check bounded metadata queries and results that outlive catalogue changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
#define DIGEST "0000000000000000000000000000000000000000000000000000000000000000"
static const char framework[] =
    "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"framework.pack\","
    "\"title\":\"Framework documentation\",\"product\":\"framework\",\"locale\":\"en-GB\",\"revision\":2,"
    "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
    "{\"id\":\"example\",\"title\":\"Copy a file\",\"topic\":\"ownership\",\"kind\":\"example\",\"path\":\"copy.c\",\"sha256\":\"" DIGEST "\"},"
    "{\"id\":\"guide\",\"title\":\"Read a guide\",\"topic\":\"navigation\",\"kind\":\"guide\",\"path\":\"guide.html\",\"sha256\":\"" DIGEST "\"},"
    "{\"id\":\"cafe\",\"title\":\"Caf\xc3\xa9 files\",\"topic\":\"text\",\"kind\":\"lesson\",\"path\":\"cafe.html\",\"sha256\":\"" DIGEST "\"}]}";
static const char trader[] =
    "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"trader.pack\","
    "\"title\":\"Trader guides\",\"product\":\"trader\",\"locale\":\"de\",\"revision\":1,"
    "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
    "{\"id\":\"account\",\"title\":\"Read account balances\",\"topic\":\"accounts\",\"kind\":\"guide\",\"path\":\"account.html\",\"sha256\":\"" DIGEST "\"},"
    "{\"id\":\"order\",\"title\":\"Paper order entry\",\"topic\":\"trading\",\"kind\":\"lesson\",\"path\":\"order.html\",\"sha256\":\"" DIGEST "\"}]}";
#undef DIGEST
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int result = 0;
    const char *mode = argv[1], *root = argv[2];
    UmiLearningShelf *shelf = NULL;
    UmiLearningPack *pack = NULL;
    UmiLearningSearch *search = NULL, *fresh = NULL;
    UmiLearningSearchQuery query = {0};
    UmiLearningSearchInfo info;
    UmiLearningSearchHit hit;
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    if (strcmp(mode, "empty-shelf") != 0) {
        CHECK(UmiLearningPackCreate(framework, sizeof(framework) - 1U, root, (UmiVersion){0U,9U,0U}, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningPackCreate(trader, sizeof(trader) - 1U, root, (UmiVersion){0U,9U,0U}, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
    }
    size_t expected = 5U, maximum = 16U;
    const char *first = "example";
    if (strcmp(mode, "browse") == 0 || strcmp(mode, "snapshot") == 0 ||
        strcmp(mode, "removed") == 0 || strcmp(mode, "outputs") == 0) {
        /* Empty text deliberately browses admitted metadata without file I/O. */
    } else if (strcmp(mode, "empty-shelf") == 0) {
        expected = 0U;
    } else if (strcmp(mode, "spaces") == 0) {
        strcpy(query.text, "   ");
    } else if (strcmp(mode, "title") == 0) {
        strcpy(query.text, "READ"); expected = 2U; first = "guide";
    } else if (strcmp(mode, "terms") == 0) {
        strcpy(query.text, "  CoPy   ownership  "); expected = 1U;
    } else if (strcmp(mode, "identity") == 0) {
        strcpy(query.text, "account"); expected = 1U; first = "account";
    } else if (strcmp(mode, "pack-title") == 0) {
        strcpy(query.text, "FRAMEWORK"); expected = 3U;
    } else if (strcmp(mode, "product") == 0) {
        strcpy(query.product, "trader"); expected = 2U; first = "account";
    } else if (strcmp(mode, "locale") == 0) {
        strcpy(query.locale, "EN-gb"); expected = 3U;
    } else if (strcmp(mode, "locale-no-fallback") == 0) {
        strcpy(query.locale, "en"); expected = 0U;
    } else if (strcmp(mode, "filters") == 0) {
        strcpy(query.product, "framework"); strcpy(query.locale, "en-GB");
        query.kind = UMI_LEARNING_RESOURCE_GUIDE; expected = 1U; first = "guide";
    } else if (strcmp(mode, "kind") == 0) {
        query.kind = UMI_LEARNING_RESOURCE_EXAMPLE; expected = 1U;
    } else if (strcmp(mode, "literal") == 0) {
        strcpy(query.text, "*"); expected = 0U;
    } else if (strcmp(mode, "all-terms") == 0) {
        strcpy(query.text, "copy balances"); expected = 0U;
    } else if (strcmp(mode, "unicode") == 0) {
        strcpy(query.text, "caf\xc3\xa9"); expected = 1U; first = "cafe";
    } else if (strcmp(mode, "unicode-case") == 0) {
        strcpy(query.text, "CAF\xc3\x89"); expected = 0U;
    } else if (strcmp(mode, "truncated") == 0) {
        maximum = 1U;
    } else if (strcmp(mode, "eight-terms") == 0) {
        strcpy(query.text, "read read read read read read read read");
        expected = 2U; first = "guide";
    } else if (strcmp(mode, "many-terms") == 0 || strcmp(mode, "unterminated") == 0 ||
               strcmp(mode, "product-unterminated") == 0 || strcmp(mode, "locale-unterminated") == 0 ||
               strcmp(mode, "control") == 0) {
        if (strcmp(mode, "many-terms") == 0) strcpy(query.text, "a b c d e f g h i");
        if (strcmp(mode, "unterminated") == 0) memset(query.text, 'a', sizeof(query.text));
        if (strcmp(mode, "product-unterminated") == 0) memset(query.product, 'a', sizeof(query.product));
        if (strcmp(mode, "locale-unterminated") == 0) memset(query.locale, 'a', sizeof(query.locale));
        if (strcmp(mode, "control") == 0) strcpy(query.text, "copy\n");
        UmiStatus wanted = strcmp(mode, "control") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(UmiLearningSearchCreate(shelf, &query, maximum, &search) == wanted && search == NULL);
        goto cleanup;
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiLearningSearchCreate(NULL, &query, maximum, &search) == UMI_STATUS_INVALID_ARGUMENT && search == NULL);
        CHECK(UmiLearningSearchCreate(shelf, NULL, maximum, &search) == UMI_STATUS_INVALID_ARGUMENT && search == NULL);
        CHECK(UmiLearningSearchCreate(shelf, &query, 0U, &search) == UMI_STATUS_INVALID_ARGUMENT && search == NULL);
        CHECK(UmiLearningSearchCreate(shelf, &query, UMI_LEARNING_SEARCH_MAX_RESULTS + 1U, &search) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningSearchCreate(shelf, &query, maximum, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        query.kind = 99U;
        CHECK(UmiLearningSearchCreate(shelf, &query, maximum, &search) == UMI_STATUS_INVALID_ARGUMENT && search == NULL);
        goto cleanup;
    } else { result = 2; goto cleanup; }
    CHECK(UmiLearningSearchCreate(shelf, &query, maximum, &search) == UMI_STATUS_OK);
    CHECK(UmiLearningSearchCopyInfo(search, &info) == UMI_STATUS_OK);
    CHECK(info.matching_resources == expected);
    CHECK(info.result_count == (expected < maximum ? expected : maximum));
    CHECK(info.truncated == (expected > maximum));
    if (expected != 0U) {
        CHECK(UmiLearningSearchResultAt(search, 0U, &hit) == UMI_STATUS_OK);
        CHECK(strcmp(hit.resource.id, first) == 0);
    }
    if (strcmp(mode, "identity") == 0) {
        CHECK((hit.matched_fields & (UMI_LEARNING_MATCH_ID | UMI_LEARNING_MATCH_TITLE | UMI_LEARNING_MATCH_TOPIC)) ==
            (UMI_LEARNING_MATCH_ID | UMI_LEARNING_MATCH_TITLE | UMI_LEARNING_MATCH_TOPIC));
    }
    if (strcmp(mode, "terms") == 0)
        CHECK((hit.matched_fields & (UMI_LEARNING_MATCH_TITLE | UMI_LEARNING_MATCH_TOPIC)) ==
            (UMI_LEARNING_MATCH_TITLE | UMI_LEARNING_MATCH_TOPIC));
    if (strcmp(mode, "snapshot") == 0) {
        UmiLearningShelfDestroy(shelf); shelf = NULL;
        CHECK(UmiLearningSearchResultAt(search, 3U, &hit) == UMI_STATUS_OK);
        CHECK(strcmp(hit.pack.id, "trader.pack") == 0 && strcmp(hit.resource.id, "account") == 0);
    }
    if (strcmp(mode, "removed") == 0) {
        CHECK(UmiLearningShelfRemove(shelf, "framework.pack") == UMI_STATUS_OK);
        CHECK(UmiLearningSearchCreate(shelf, &query, maximum, &fresh) == UMI_STATUS_OK);
        CHECK(UmiLearningSearchCopyInfo(fresh, &info) == UMI_STATUS_OK && info.result_count == 2U);
        CHECK(UmiLearningSearchResultAt(fresh, 0U, &hit) == UMI_STATUS_OK && strcmp(hit.pack.id, "trader.pack") == 0);
        CHECK(UmiLearningSearchCopyInfo(search, &info) == UMI_STATUS_OK && info.result_count == 5U);
    }
    if (strcmp(mode, "outputs") == 0) {
        /* Keep the previous assignment snapshots for review. memcpy below
         * preserves padding too, so memcmp checks the untouched output bytes. */
#if 0
        hit = original;
        resource = guard;
#endif
        UmiLearningSearchHit original;
        memset(&original, 0x5a, sizeof(original)); memcpy(&hit, &original, sizeof(hit));
        CHECK(UmiLearningSearchResultAt(search, SIZE_MAX, &hit) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&original, &hit, sizeof(hit)) == 0);
        CHECK(UmiLearningSearchResultAt(NULL, 0U, &hit) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningSearchCopyInfo(NULL, &info) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningSearchCopyInfo(search, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiLearningPackResource resource, guard;
        memset(&guard, 0x5a, sizeof(guard)); memcpy(&resource, &guard, sizeof(resource));
        CHECK(UmiLearningShelfResourceAt(shelf, "missing", 0U, &resource) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&resource, &guard, sizeof(guard)) == 0);
        CHECK(UmiLearningShelfResourceAt(shelf, "framework.pack", SIZE_MAX, &resource) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&resource, &guard, sizeof(guard)) == 0);
        CHECK(UmiLearningShelfResourceAt(NULL, "framework.pack", 0U, &resource) == UMI_STATUS_INVALID_ARGUMENT);
    }
cleanup:
    UmiLearningSearchDestroy(search);
    UmiLearningSearchDestroy(fresh);
    UmiLearningShelfDestroy(shelf);
    UmiLearningPackDestroy(pack);
    return result;
}
