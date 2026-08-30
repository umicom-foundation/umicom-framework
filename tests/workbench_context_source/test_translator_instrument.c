/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_translator_instrument.c
 *
 * PURPOSE:
 *   Verify Trader watchlist samples translate into instrument events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_source/translator.h"

int main(void)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiWorkbenchContextSourceSample sample;
    UmiWorkbenchContextEvent event;
    umi_workbench_context_source_definition_init(
        &definition, "trader.watchlist.selection");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.trader",
        "trader.watchlist", "Watchlist") == UMI_STATUS_OK);
    definition.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST;
    definition.context_kind = UMI_CONTEXT_KIND_INSTRUMENT;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)UMI_CONTEXT_KIND_INSTRUMENT - 1U);

    umi_workbench_context_source_sample_init(
        &sample, UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_INSTRUMENT, "instrument-event");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "trader.watchlist.selection",
        "org.umicom.trader", "trader.watchlist",
        NULL) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_subject(
        &sample, "ESU6", "ES") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_add_metadata(
        &sample, "symbol", "ES") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_add_metadata(
        &sample, "venue", "CME") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_translate(
        &definition, &sample, &event) == UMI_STATUS_OK);
    assert(event.kind ==
        UMI_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_SELECTION);
    assert(event.context_kind == UMI_CONTEXT_KIND_INSTRUMENT);

    return 0;
}
