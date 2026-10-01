/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/test_document.c
 * PURPOSE: Exercise complete JSON lexical validation and bounded failure before field interpretation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/language_runtime/json_document.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiLanguageRuntimeJsonDocument *doc = calloc(1, sizeof *doc), *before = malloc(sizeof *before);
    CHECK(doc != NULL && before != NULL); OK(UmiLanguageRuntimeJsonParseComplete("{\"old\":3}", doc)); *before = *doc;
    if (strcmp(name, "grammar") == 0) {
        const char *valid[] = {"0", "-0", "-1", "1.5", "1e3", "1E-3", "1e+3", "true", "false", "null"};
        const char *invalid[] = {"+1", "01", "--1", "1.", "1e", "1e+", "nan", "truefalse", "undefined", ".3"};
        for (size_t i = 0U; i < sizeof valid / sizeof valid[0]; ++i) OK(UmiLanguageRuntimeJsonParseComplete(valid[i], doc));
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
            *before = *doc; CHECK(UmiLanguageRuntimeJsonParseComplete(invalid[i], doc) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(doc, before, sizeof *doc) == 0);
        }
    } else if (strcmp(name, "extension-invalid") == 0 || strcmp(name, "extension-unicode") == 0) {
        UmiDebugVariableChildren *children = MakeChildren(1U);
        const char *json = strcmp(name, "extension-invalid") == 0 ?
            "{\"body\":{\"variables\":[],\"extension\":undefined}}" :
            "{\"body\":{\"variables\":[],\"extension\":\"\\ud800\"}}";
        CHECK(UmiDebugRuntimeDecodeVariableChildren(json, children) == UMI_STATUS_PARSE_ERROR);
        CHECK(children->count == 1U && children->items[0].variables_reference == 1U); free(children);
    } else if (strcmp(name, "whitespace") == 0) {
        OK(UmiLanguageRuntimeJsonParseComplete(" \t\r\n{\"a\":3}\n", doc)); *before = *doc;
        CHECK(UmiLanguageRuntimeJsonParseComplete("\v{}", doc) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(doc, before, sizeof *doc) == 0);
        CHECK(UmiLanguageRuntimeJsonParseComplete("{\"a\":\f3}", doc) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "depth") == 0 || strcmp(name, "depth-limit") == 0) {
        char json[512]; size_t n = UMI_LANGUAGE_RUNTIME_JSON_MAX_DEPTH + (strcmp(name, "depth-limit") == 0 ? 1U : 0U);
        memset(json, '[', n); memset(json + n, ']', n); json[n * 2U] = '\0';
        UmiStatus status = UmiLanguageRuntimeJsonParseComplete(json, doc);
        CHECK(status == (n > UMI_LANGUAGE_RUNTIME_JSON_MAX_DEPTH ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK));
        if (status != UMI_STATUS_OK) CHECK(memcmp(doc, before, sizeof *doc) == 0);
    } else if (strcmp(name, "byte-limit") == 0 || strcmp(name, "token-limit") == 0) {
        char *json = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY + 1U); CHECK(json != NULL);
        if (strcmp(name, "byte-limit") == 0) {
            memset(json, ' ', UMI_LANGUAGE_RUNTIME_JSON_CAPACITY); json[0] = '0'; json[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY] = '\0';
        } else {
            size_t offset = 0U; json[offset++] = '[';
            for (size_t i = 0U; i < UMI_LANGUAGE_RUNTIME_MAX_TOKENS; ++i) { if (i != 0U) json[offset++] = ','; json[offset++] = '0'; }
            json[offset++] = ']'; json[offset] = '\0';
        }
        CHECK(UmiLanguageRuntimeJsonParseComplete(json, doc) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(doc, before, sizeof *doc) == 0); free(json);
    } else {
        CHECK(strcmp(name, "atomic") == 0);
        CHECK(UmiLanguageRuntimeJsonParseComplete("{\"a\":3}garbage", doc) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(doc, before, sizeof *doc) == 0);
        CHECK(UmiLanguageRuntimeJsonParseComplete(NULL, doc) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(doc, before, sizeof *doc) == 0);
    }
    free(before); free(doc); return 0;
}
