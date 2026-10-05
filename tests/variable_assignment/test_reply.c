/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_assignment/test_reply.c
 * PURPOSE: Check assignment reply grammar, optional metadata, limits and atomic refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../variable_inspection/fixture.h"
#include "umicom/debug_runtime/variable_assignment.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; const char *json = NULL;
    UmiStatus expected = UMI_STATUS_PARSE_ERROR; char buffer[8192];
    UmiDebugRuntimeEvaluateResult result, before; memset(&result, 0x5a, sizeof result); before = result;
    if (strcmp(name, "valid") == 0) { expected = UMI_STATUS_OK;
        json = "{\"body\":{\"value\":\"7\",\"type\":\"int\",\"variablesReference\":3,\"namedVariables\":4,\"indexedVariables\":5,\"memoryReference\":\"0xff\"}}";
    } else if (strcmp(name, "empty") == 0) { expected = UMI_STATUS_OK; json = "{\"body\":{\"value\":\"\"}}";
    } else if (strcmp(name, "unicode") == 0) { expected = UMI_STATUS_OK; json = "{\"body\":{\"value\":\"caf\\u00e9\"}}";
    } else if (strcmp(name, "evaluate-field") == 0) json = "{\"body\":{\"result\":\"7\"}}";
    else if (strcmp(name, "wrong-value") == 0) json = "{\"body\":{\"value\":7}}";
    else if (strcmp(name, "wrong-type") == 0) json = "{\"body\":{\"value\":\"7\",\"type\":null}}";
    else if (strcmp(name, "duplicate") == 0) json = "{\"body\":{\"value\":\"7\",\"v\\u0061lue\":\"8\"}}";
    else if (strcmp(name, "duplicate-body") == 0) json = "{\"body\":{\"value\":\"7\"},\"body\":{\"value\":\"8\"}}";
    else if (strcmp(name, "duplicate-reference") == 0) json = "{\"body\":{\"value\":\"7\",\"variablesReference\":0,\"variablesReference\":1}}";
    else if (strcmp(name, "negative") == 0) json = "{\"body\":{\"value\":\"7\",\"namedVariables\":-1}}";
    else if (strcmp(name, "fraction") == 0) json = "{\"body\":{\"value\":\"7\",\"variablesReference\":1.5}}";
    else if (strcmp(name, "overflow") == 0) { expected = UMI_STATUS_CAPACITY_EXCEEDED; json = "{\"body\":{\"value\":\"7\",\"variablesReference\":2147483648}}";
    } else if (strcmp(name, "boundary-reference") == 0) { expected = UMI_STATUS_OK; json = "{\"body\":{\"value\":\"7\",\"variablesReference\":2147483647}}";
    } else if (strcmp(name, "nul") == 0) json = "{\"body\":{\"value\":\"x\\u0000y\"}}";
    else if (strcmp(name, "surrogate") == 0) json = "{\"body\":{\"value\":\"\\ud800\"}}";
    else if (strcmp(name, "trailing") == 0) json = "{\"body\":{\"value\":\"7\"}} false";
    else if (strcmp(name, "extension") == 0) { expected = UMI_STATUS_OK; json = "{\"body\":{\"value\":\"7\",\"vendor\":{\"hint\":true}}}";
    } else if (strcmp(name, "boundary-value") == 0 || strcmp(name, "oversized") == 0) {
        size_t length = strcmp(name, "oversized") == 0 ? 4096U : 4095U;
        char value[4097]; memset(value, 'x', length); value[length] = '\0';
        (void)snprintf(buffer, sizeof buffer, "{\"body\":{\"value\":\"%s\"}}", value); json = buffer;
        expected = length == 4095U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    } else CHECK(0);
    CHECK(UmiDebugRuntimeDecodeAssignmentValue(json, &result) == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(&result, &before, sizeof result) == 0);
    else if (strcmp(name, "valid") == 0) CHECK(strcmp(result.result, "7") == 0 && strcmp(result.type, "int") == 0 &&
        result.variables_reference == 3U && result.named_variables == 4U && result.indexed_variables == 5U && strcmp(result.memory_reference, "0xff") == 0);
    else if (strcmp(name, "unicode") == 0) CHECK(strcmp(result.result, "caf\xc3\xa9") == 0);
    else if (strcmp(name, "empty") == 0) CHECK(result.result[0] == '\0' && result.type[0] == '\0' && result.variables_reference == 0U);
    else if (strcmp(name, "boundary-reference") == 0) CHECK(result.variables_reference == INT32_MAX);
    else if (strcmp(name, "boundary-value") == 0) CHECK(strlen(result.result) == 4095U);
    else CHECK(strcmp(result.result, "7") == 0);
    return 0;
}
