/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/test_protocol.c
 * PURPOSE: Check capability negotiation, exact payload escaping and malformed reply rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../debug_runtime/request_test_support.h"
#include "umicom/debug_runtime/breakpoint_sync.h"
#include "umicom/debug_runtime/capabilities.h"
#include "umicom/debug_runtime/decoders/initialize.h"
#include "umicom/debug_runtime/decoders/breakpoints.h"
#include "umicom/debug_runtime/requests/set_breakpoints.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    if (strcmp(name, "capabilities") == 0 || strcmp(name, "disabled") == 0) {
        UmiDebugRuntimeCapabilities caps = {0}; UmiDebugBreakpointSnapshot item = {0};
        item.enabled = 1; item.line = 13; strcpy(item.condition, "count > 3"); strcpy(item.log_message, "count={count}");
        CHECK(UmiDebugRuntimeBreakpointSetValidate(&caps, &item, 1U) == UMI_STATUS_UNAVAILABLE);
        caps.supports_conditional_breakpoints = 1;
        CHECK(UmiDebugRuntimeBreakpointSetValidate(&caps, &item, 1U) == UMI_STATUS_UNAVAILABLE);
        if (strcmp(name, "disabled") == 0) { item.enabled = 0; item.line = 0; OK(UmiDebugRuntimeBreakpointSetValidate(&caps, &item, 1U)); }
        else {
            OK(umi_debug_runtime_decode_initialize("{\"body\":{\"supportsConditionalBreakpoints\":true,\"supportsLogPoints\":true}}", &caps));
            CHECK(caps.supports_log_points && (umi_debug_runtime_capability_bits(&caps) & UMI_DEBUG_CAP_LOG_POINTS));
            OK(UmiDebugRuntimeBreakpointSetValidate(&caps, &item, 1U));
            OK(umi_debug_runtime_decode_initialize("{\"body\":{}}", &caps)); CHECK(!caps.supports_log_points);
        }
    } else if (strcmp(name, "request") == 0 || strcmp(name, "invalid-request") == 0) {
        DebugRequestTestFixture f; OK(debug_request_test_fixture_create(&f));
        UmiDebugBreakpointSnapshot items[2] = {{0}}; items[1].enabled = 1; items[1].line = 13;
        strcpy(items[1].condition, "name == \"caf\xc3\xa9\""); strcpy(items[1].log_message, "x={x}\nnext");
        uint64_t sequence = 0; char *written = malloc(UMI_DEBUG_RUNTIME_FRAME_CAPACITY); CHECK(written != NULL);
        if (strcmp(name, "invalid-request") == 0) {
            items[1].line = UINT32_MAX;
            CHECK(umi_debug_runtime_request_set_breakpoints(f.adapter, "main.c", items, 2, 0, &sequence) == UMI_STATUS_INVALID_ARGUMENT && sequence == 0U);
        } else {
            OK(umi_debug_runtime_request_set_breakpoints(f.adapter, "main.c", items, 2, 0, &sequence));
            OK(debug_request_test_fixture_written(&f, written, UMI_DEBUG_RUNTIME_FRAME_CAPACITY));
            CHECK(strstr(written, "\"breakpoints\":[{\"line\":13,") != NULL && strstr(written, "\"logMessage\":\"x={x}\\nnext\"") != NULL);
            CHECK(strstr(written, "caf\xc3\xa9\\\"") != NULL && sequence != 0U);
        }
        free(written); debug_request_test_fixture_destroy(&f);
    } else if (strcmp(name, "excess-count") == 0) {
        char json[8192]; strcpy(json, "{\"body\":{\"breakpoints\":[");
        for (size_t i = 0U; i <= UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS; ++i)
            strcat(json, i == 0U ? "{\"verified\":true}" : ",{\"verified\":true}");
        strcat(json, "]}}");
        UmiDebugRuntimeBreakpointList *reply = calloc(1U, sizeof *reply); CHECK(reply != NULL);
        CHECK(umi_debug_runtime_decode_breakpoints(json, reply) == UMI_STATUS_CAPACITY_EXCEEDED && reply->count == 0U);
        free(reply);
    } else {
        UmiDebugRuntimeBreakpointList *reply = calloc(1U, sizeof *reply); CHECK(reply != NULL);
        const char *json = NULL;
        if (strcmp(name, "missing-verified") == 0) json = "{\"body\":{\"breakpoints\":[{\"line\":13}]}}";
        else if (strcmp(name, "wrong-verified") == 0) json = "{\"body\":{\"breakpoints\":[{\"verified\":\"true\"}]}}";
        else if (strcmp(name, "wrong-array") == 0) json = "{\"body\":{\"breakpoints\":{}}}";
        else if (strcmp(name, "negative-line") == 0) json = "{\"body\":{\"breakpoints\":[{\"verified\":true,\"line\":-1}]}}";
        else if (strcmp(name, "overflow-line") == 0) json = "{\"body\":{\"breakpoints\":[{\"verified\":true,\"line\":4294967297}]}}";
        else if (strcmp(name, "wrong-row") == 0) json = "{\"body\":{\"breakpoints\":[false]}}";
        else return 2;
        CHECK(umi_debug_runtime_decode_breakpoints(json, reply) == UMI_STATUS_PARSE_ERROR); free(reply);
    }
    return 0;
}
