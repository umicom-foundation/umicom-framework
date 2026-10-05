/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_setup/test_model.c
 * PURPOSE: Check owned settings, complete capture and validation without launching a debugger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiDebugSetup *setup = Setup(), *copy = NULL;
    UmiDebugSetupBreakpoint point;
    UmiDebugSetupWatch watch;
    UmiDebugSetupSummary summary;
    OK(UmiDebugSetupBreakpointAt(setup, 0U, &point));
    OK(UmiDebugSetupWatchAt(setup, 0U, &watch));
    if (strcmp(mode, "copy") == 0)
    {
        OK(UmiDebugSetupCopy(setup, &copy));
        UmiDebugSetupDestroy(setup);
        setup = NULL;
        OK(UmiDebugSetupInspect(copy, &summary));
        CHECK(summary.breakpoints == 1U && summary.watches == 1U);
        UmiDebugSetupBreakpoint stored;
        OK(UmiDebugSetupBreakpointAt(copy, 0U, &stored));
        CHECK(strcmp(stored.condition, point.condition) == 0);
    }
    else if (strcmp(mode, "capture") == 0 || strcmp(mode, "capture-large") == 0)
    {
        Fixture f = {0};
        Open(&f);
        SeedWatch(&f);
        if (strcmp(mode, "capture-large") == 0)
        {
            for (size_t i = 0U; i < UMI_DEBUG_SETUP_CAPACITY; ++i)
            {
                UmiDebugBreakpointSnapshot value = Read(&f);
                (void)snprintf(value.id, sizeof value.id, "extra-%zu", i);
                value.line = (uint32_t)(30U + i);
                OK(umi_debug_breakpoint_registry_upsert(f.registry, &value));
            }
            CHECK(UmiDebugSetupCapture(f.workspace, "Too large", &copy) == UMI_STATUS_CAPACITY_EXCEEDED &&
                  copy == NULL);
        }
        else
        {
            OK(UmiDebugSetupCapture(f.workspace, "Captured", &copy));
            Close(&f);
            char *bytes = NULL;
            size_t size;
            OK(UmiDebugSetupEncode(copy, &bytes, &size));
            CHECK(strstr(bytes, "private runtime value") == NULL &&
                  strstr(bytes, "previous-process") == NULL && strstr(bytes, "verified") == NULL);
            CHECK(strstr(bytes, "count") != NULL && size > 0U);
            UmiDebugSetupFreeBytes(bytes);
        }
        Close(&f);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        for (size_t i = 1U; i < UMI_DEBUG_SETUP_CAPACITY; ++i)
        {
            point.line = (uint32_t)(100U + i);
            OK(UmiDebugSetupAddBreakpoint(setup, &point));
            OK(UmiDebugSetupAddWatch(setup, &watch));
        }
        point.line = 999U;
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDebugSetupAddWatch(setup, &watch) == UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiDebugSetupInspect(setup, &summary));
        CHECK(summary.breakpoints == 64U && summary.watches == 64U);
    }
    else if (strcmp(mode, "duplicates") == 0)
    {
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_ALREADY_EXISTS);
        point.column++;
        OK(UmiDebugSetupAddBreakpoint(setup, &point));
        OK(UmiDebugSetupAddWatch(setup, &watch));
    }
    else if (strcmp(mode, "bounds") == 0)
    {
        UmiDebugSetupBreakpoint original = point;
        CHECK(UmiDebugSetupBreakpointAt(setup, 1U, &point) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&point, &original, sizeof point) == 0);
        memset(point.source, 'x', sizeof point.source);
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_CAPACITY_EXCEEDED);
        memset(watch.expression, 'x', sizeof watch.expression);
        CHECK(UmiDebugSetupAddWatch(setup, &watch) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        point.line = 0U;
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_INVALID_ARGUMENT);
        point.line = UINT32_MAX;
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_INVALID_ARGUMENT);
        point.line = 2U;
        point.enabled = 2;
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_INVALID_ARGUMENT);
        watch.expression[0] = '\0';
        CHECK(UmiDebugSetupAddWatch(setup, &watch) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        strcpy(point.source, "/home/caf\xc3\xa9/main.c");
        OK(UmiDebugSetupAddBreakpoint(setup, &point));
        point.source[0] = (char)0xff;
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_INVALID_ARGUMENT);
        strcpy(watch.expression, "x\n+1");
        OK(UmiDebugSetupAddWatch(setup, &watch));
        strcpy(point.source, "bad\nsource");
        CHECK(UmiDebugSetupAddBreakpoint(setup, &point) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
        return 2;
    UmiDebugSetupDestroy(copy);
    UmiDebugSetupDestroy(setup);
    return 0;
}
