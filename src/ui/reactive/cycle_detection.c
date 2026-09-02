/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/cycle_detection.c
 *
 * PURPOSE:
 *   Detect cyclic state dependencies before propagation is enabled.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/cycle_detection.h"
#include "umicom/ui/reactive/dependency_graph.h"
#include <string.h>
/* Provide the visit operation used by this module and its client applications. */
static int visit(const UmiUiReactiveDependencyGraph *g,uint16_t n,unsigned char *state,size_t *visited){ size_t e; /* Apply this operation only while the related capability or state is available. */ if(state[n]==1U)return 1; /* Apply this operation only while the related capability or state is available. */ if(state[n]==2U)return 0; state[n]=1U;(*visited)++;/* Visit each bounded item once so every record receives the same rule. */ for(e=0;e<g->edge_count;e++)/* Apply this operation only while the related capability or state is available. */ if(g->from_index[e]==n&&visit(g,g->to_index[e],state,visited))return 1;state[n]=2U;return 0; }
/* Detect a dependency cycle with a bounded depth-first traversal. */
UmiStatus umi_ui_reactive_cycle_detection_run(const UmiUiReactiveDependencyGraph *g,UmiUiReactiveCycleDetection *out){ unsigned char state[UMI_UI_REACTIVE_MAX_ITEMS];size_t i,visited=0; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!g||!out)return UMI_STATUS_INVALID_ARGUMENT; memset(state,0,sizeof state);out->has_cycle=false;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<g->node_count;i++)/* Preserve the original failure result so the caller can respond to the correct cause. */ if(visit(g,(uint16_t)i,state,&visited)){out->has_cycle=true;break;}out->visited_nodes=visited;return UMI_STATUS_OK; }
