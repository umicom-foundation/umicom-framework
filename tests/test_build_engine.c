/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_engine.c
 * PURPOSE: Verify engine graph ownership, cancellation and incremental state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/build/engine.h"

int main(void)
{
    UmiBuildEngine *engine = NULL;
    UmiBuildEngineConfig config = {0};
    UmiBuildEngineSnapshot snapshot;
    UmiBuildGraphNodeSnapshot node;
    UmiClock clock = umi_clock_system();
    umi_build_profile_init(&config.profile);
    config.clock = &clock;
    assert(umi_build_engine_create(&config, &engine) == UMI_STATUS_OK);
    umi_build_graph_node_init(&node, "configure", "Configure",
                              UMI_BUILD_PHASE_CONFIGURE);
    node.incremental = 1;
    assert(umi_build_graph_add_node(umi_build_engine_graph(engine), &node) ==
           UMI_STATUS_OK);
    umi_build_engine_request_cancel(engine);
    assert(umi_build_engine_snapshot(engine, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.graph.node_count == 1U);
    assert(snapshot.cancellation_requested == 1);
    umi_build_engine_reset_cancel(engine);
    assert(umi_build_engine_invalidate(engine, "configure", 2U) ==
           UMI_STATUS_OK);
    assert(umi_build_engine_snapshot(engine, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.cancellation_requested == 0);
    umi_build_engine_destroy(engine);
    umi_clock_dispose(&clock);
    return 0;
}
