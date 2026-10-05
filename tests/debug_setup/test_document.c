/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_setup/test_document.c
 * PURPOSE: Exercise strict document schemas and complete output ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiDebugSetup *setup = Setup(), *decoded = NULL;
    char *bytes = NULL;
    size_t size = 0U;
    OK(UmiDebugSetupEncode(setup, &bytes, &size));
    if (strcmp(mode, "round-trip") == 0 || strcmp(mode, "maximum") == 0 || strcmp(mode, "escaping") == 0)
    {
        if (strcmp(mode, "maximum") == 0)
        {
            UmiDebugSetupBreakpoint point;
            UmiDebugSetupWatch watch;
            OK(UmiDebugSetupBreakpointAt(setup, 0U, &point));
            memset(point.condition, '"', sizeof point.condition - 1U);
            point.condition[sizeof point.condition - 1U] = '\0';
            memset(point.source, 'x', sizeof point.source - 1U);
            point.source[sizeof point.source - 1U] = '\0';
            OK(UmiDebugSetupWatchAt(setup, 0U, &watch));
            memset(watch.expression, '"', sizeof watch.expression - 1U);
            watch.expression[sizeof watch.expression - 1U] = '\0';
            for (size_t i = 1U; i < UMI_DEBUG_SETUP_CAPACITY; ++i)
            {
                point.line = (uint32_t)i;
                OK(UmiDebugSetupAddBreakpoint(setup, &point));
                OK(UmiDebugSetupAddWatch(setup, &watch));
            }
        }
        if (strcmp(mode, "escaping") == 0)
        {
            UmiDebugSetupWatch watch = {0};
            strcpy(watch.expression, "name==\"caf\xc3\xa9\"\n&& path==\"C:\\\\data\"");
            watch.enabled = 1;
            OK(UmiDebugSetupAddWatch(setup, &watch));
        }
        UmiDebugSetupFreeBytes(bytes);
        bytes = NULL;
        OK(UmiDebugSetupEncode(setup, &bytes, &size));
        OK(UmiDebugSetupDecode(bytes, size, &decoded));
        char *again = NULL;
        size_t againSize = 0U;
        OK(UmiDebugSetupEncode(decoded, &again, &againSize));
        CHECK(size == againSize && memcmp(bytes, again, size) == 0);
        UmiDebugSetupFreeBytes(again);
    }
    else
    {
        const char *json = NULL;
        if (strcmp(mode, "unknown") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"breakpoints\":[],\"watches\":[],"
                   "\"command\":\"run\"}";
        else if (strcmp(mode, "duplicate") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"title\":\"y\",\"watches\":[]}";
        else if (strcmp(mode, "missing") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"breakpoints\":[],\"watches\":[]}";
        else if (strcmp(mode, "format") == 0)
            json = "{\"format\":\"other\",\"title\":\"x\",\"breakpoints\":[],\"watches\":[]}";
        else if (strcmp(mode, "wrong-kind") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"breakpoints\":{},\"watches\":[]}";
        else if (strcmp(mode, "boolean") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"breakpoints\":[],\"watches\":[{"
                   "\"expression\":\"x\",\"enabled\":1}]}";
        else if (strcmp(mode, "late-invalid") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"breakpoints\":[],\"watches\":[{"
                   "\"expression\":\"x\",\"enabled\":true},{\"expression\":\"\",\"enabled\":true}]}";
        else if (strcmp(mode, "embedded-nul") == 0)
        {
            bytes[size / 2U] = '\0';
        }
        else if (strcmp(mode, "trailing") == 0)
        {
            bytes[size] = 'x';
            ++size;
        }
        else if (strcmp(mode, "truncated") == 0)
        {
            size /= 2U;
        }
        else if (strcmp(mode, "line-overflow") == 0)
            json = "{\"format\":\"umicom.debug-setup\",\"title\":\"x\",\"breakpoints\":[{\"source\":\"a\","
                   "\"line\":2147483648,\"column\":0,\"enabled\":true,\"condition\":\"\",\"logMessage\":\"\"}"
                   "],\"watches\":[]}";
        else
            return 2;
        CHECK(UmiDebugSetupDecode(json != NULL ? json : bytes, json != NULL ? strlen(json) : size,
                                  &decoded) != UMI_STATUS_OK &&
              decoded == NULL);
    }
    UmiDebugSetupFreeBytes(bytes);
    UmiDebugSetupDestroy(decoded);
    UmiDebugSetupDestroy(setup);
    return 0;
}
