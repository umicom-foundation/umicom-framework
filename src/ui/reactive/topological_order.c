/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/topological_order.c
 *
 * PURPOSE:
 *   Produce deterministic dependency order for computed-state propagation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/topological_order.h"
#include "umicom/ui/reactive/dependency_graph.h"
#include <string.h>
/* Produce Kahn topological order; complete=false identifies a dependency cycle. */
UmiStatus umi_ui_reactive_topological_order_build(const UmiUiReactiveDependencyGraph *g,UmiUiReactiveTopologicalOrder *out){ uint16_t indeg[UMI_UI_REACTIVE_MAX_ITEMS];unsigned char used[UMI_UI_REACTIVE_MAX_ITEMS];size_t i,e,k; if(!g||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(indeg,0,sizeof indeg);memset(used,0,sizeof used);memset(out,0,sizeof *out);for(e=0;e<g->edge_count;e++)indeg[g->to_index[e]]++;for(k=0;k<g->node_count;k++){int found=-1;for(i=0;i<g->node_count;i++)if(!used[i]&&indeg[i]==0U){found=(int)i;break;}if(found<0)break;used[(size_t)found]=1U;out->order[out->count++]=(uint16_t)found;for(e=0;e<g->edge_count;e++)if(g->from_index[e]==(uint16_t)found&&indeg[g->to_index[e]]>0U)indeg[g->to_index[e]]--; }out->complete=(out->count==g->node_count);return UMI_STATUS_OK; }
