/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/test_reply.c
 * PURPOSE: Reject incomplete or ambiguous child responses without modifying the prior output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiDebugVariableChildren *out = MakeChildren(77U), *before = malloc(sizeof *before); CHECK(before != NULL); *before = *out;
    char *json = calloc(1, 65536U); CHECK(json != NULL); const char *body = NULL;
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(name, "valid") == 0) { expected = UMI_STATUS_OK;
        body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"type\":\"int\",\"evaluateName\":\"notes.item\",\"variablesReference\":2,\"namedVariables\":3,\"indexedVariables\":4,\"memoryReference\":\"0xff\",\"vendorHint\":true}]}";
    } else if (strcmp(name, "unicode") == 0) { expected = UMI_STATUS_OK;
        body = "{\"variables\":[{\"name\":\"caf\\u00e9\",\"value\":\"\\ud83d\\ude80\",\"variablesReference\":0}]}";
    } else if (strcmp(name, "empty") == 0) { expected = UMI_STATUS_OK; body = "{\"variables\":[]}";
    } else if (strcmp(name, "empty-value") == 0) { expected = UMI_STATUS_OK;
        body = "{\"variables\":[{\"name\":\"\",\"value\":\"\",\"variablesReference\":0}]}";
    } else if (strcmp(name, "missing") == 0) body = "{}";
    else if (strcmp(name, "array-type") == 0) body = "{\"variables\":{}}";
    else if (strcmp(name, "row-type") == 0) body = "{\"variables\":[4]}";
    else if (strcmp(name, "missing-value") == 0) body = "{\"variables\":[{\"name\":\"item\",\"variablesReference\":0}]}";
    else if (strcmp(name, "missing-reference") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\"}]}";
    else if (strcmp(name, "duplicate") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"v\\u0061lue\":\"4\",\"variablesReference\":0}]}";
    else if (strcmp(name, "duplicate-array") == 0) body = "{\"variables\":[],\"variables\":[]}";
    else if (strcmp(name, "nul") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"x\\u0000y\",\"variablesReference\":0}]}";
    else if (strcmp(name, "surrogate") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"\\ud800\",\"variablesReference\":0}]}";
    else if (strcmp(name, "optional-type") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"type\":null,\"variablesReference\":0}]}";
    else if (strcmp(name, "negative-count") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":0,\"indexedVariables\":-1}]}";
    else if (strcmp(name, "negative-reference") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":-1}]}";
    else if (strcmp(name, "fraction") == 0) body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":1.5}]}";
    else if (strcmp(name, "overflow") == 0) { expected = UMI_STATUS_CAPACITY_EXCEEDED;
        body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":999999999999999999999999}]}";
    } else if (strcmp(name, "boundary-reference") == 0) { expected = UMI_STATUS_OK;
        body = "{\"variables\":[{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":2147483647}]}";
    } else if (strcmp(name, "long-value") == 0 || strcmp(name, "boundary-value") == 0) {
        size_t length = strcmp(name, "long-value") == 0 ? 4096U : 4095U;
        char *value = malloc(length + 1U); CHECK(value != NULL); memset(value, 'x', length); value[length] = '\0';
        (void)snprintf(json, 65536U, "{\"body\":{\"variables\":[{\"name\":\"item\",\"value\":\"%s\",\"variablesReference\":0}]}}", value);
        free(value); expected = length == 4095U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "capacity") == 0 || strcmp(name, "oversized") == 0 || strcmp(name, "names") == 0) {
        size_t count = strcmp(name, "names") == 0 ? 2U : UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY + (strcmp(name, "oversized") == 0 ? 1U : 0U);
        strcpy(json, "{\"body\":{\"variables\":[");
        for (size_t i = 0U; i < count; ++i) strcat(json, i == 0U ?
            "{\"name\":\"item\",\"value\":\"3\",\"variablesReference\":0}" : ", {\"name\":\"item\",\"value\":\"3\",\"variablesReference\":0}");
        strcat(json, "]}}"); expected = count > UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    } else if (strcmp(name, "duplicate-body") == 0) strcpy(json, "{\"body\":{\"variables\":[]},\"body\":{\"variables\":[]}}");
    else CHECK(0);
    if (body != NULL) (void)snprintf(json, 65536U, "{\"body\":%s}", body);
    CHECK(UmiDebugRuntimeDecodeVariableChildren(json, out) == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(out, before, sizeof *out) == 0);
    else if (strcmp(name, "empty") == 0) CHECK(out->count == 0U);
    else {
        CHECK(out->count > 0U);
        if (strcmp(name, "unicode") == 0) CHECK(strcmp(out->items[0].name, "caf\xc3\xa9") == 0 && strcmp(out->items[0].value, "\xf0\x9f\x9a\x80") == 0);
        if (strcmp(name, "valid") == 0) CHECK(out->items[0].named_variables == 3U && out->items[0].indexed_variables == 4U && strcmp(out->items[0].evaluate_name, "notes.item") == 0);
        if (strcmp(name, "boundary-value") == 0) CHECK(strlen(out->items[0].value) == 4095U);
        if (strcmp(name, "boundary-reference") == 0) CHECK(out->items[0].variables_reference == INT32_MAX);
        if (strcmp(name, "capacity") == 0) CHECK(out->count == UMI_DEBUG_VARIABLE_CHILDREN_CAPACITY);
        if (strcmp(name, "names") == 0) CHECK(out->count == 2U);
        if (strcmp(name, "empty-value") == 0) CHECK(out->items[0].value[0] == '\0');
    }
    free(json); free(before); free(out); return 0;
}
