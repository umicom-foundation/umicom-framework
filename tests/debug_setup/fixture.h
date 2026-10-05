/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_setup/fixture.h
 * PURPOSE: Use canonical debugger registries while keeping tests free of real adapters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SETUP_TEST_FIXTURE_H
#define UMICOM_DEBUG_SETUP_TEST_FIXTURE_H
#include "../breakpoint_edit/fixture.h"
#include "umicom/debug/setup_document.h"
static inline UmiDebugSetup *Setup(void)
{
    UmiDebugSetup *setup = NULL;
    OK(UmiDebugSetupCreate("Review caf\xc3\xa9", &setup));
    UmiDebugSetupBreakpoint point = {0};
    strcpy(point.source, "C:/source/second.c");
    point.line = 29U;
    point.column = 3U;
    strcpy(point.condition, "count > 8");
    strcpy(point.logMessage, "count={count}");
    point.enabled = 0;
    OK(UmiDebugSetupAddBreakpoint(setup, &point));
    UmiDebugSetupWatch watch = {0};
    strcpy(watch.expression, "items[3].price");
    watch.enabled = 1;
    OK(UmiDebugSetupAddWatch(setup, &watch));
    return setup;
}
static inline void SeedWatch(Fixture *f)
{
    UmiDebugWatchSnapshot watch = {0};
    strcpy(watch.id, "old-watch");
    strcpy(watch.expression, "count");
    strcpy(watch.value, "private runtime value");
    strcpy(watch.type, "int");
    strcpy(watch.session_id, "previous-process");
    watch.enabled = 1;
    watch.valid = 1;
    OK(umi_debug_watch_registry_upsert(umi_debug_service_watch(f->service), &watch));
}
#endif
