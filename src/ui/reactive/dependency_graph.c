/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/dependency_graph.c
 *
 * PURPOSE:
 *   Implement bounded dependency nodes and directed edges.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_graph.h"
#include <string.h>
/* Initialise an empty dependency graph. */
void umi_ui_reactive_dependency_graph_init(UmiUiReactiveDependencyGraph *g){ if(g) memset(g,0,sizeof *g); }
/* Add a unique node and return its stable index. */
UmiStatus umi_ui_reactive_dependency_graph_add_node(UmiUiReactiveDependencyGraph *g,const char *id,uint16_t *out_index){ size_t i,n; if(!g||!id||!out_index||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; for(i=0;i<g->node_count;i++) if(strcmp(g->node_ids[i],id)==0){*out_index=(uint16_t)i;return UMI_STATUS_ALREADY_EXISTS;} if(g->node_count>=UMI_UI_REACTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; n=strlen(id); if(n>=UMI_UI_REACTIVE_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT; memcpy(g->node_ids[g->node_count],id,n+1U); *out_index=(uint16_t)g->node_count; g->node_count++; return UMI_STATUS_OK; }
/* Add a directed dependency edge between existing node indexes. */
UmiStatus umi_ui_reactive_dependency_graph_add_edge(UmiUiReactiveDependencyGraph *g,uint16_t from,uint16_t to){ if(!g||from>=g->node_count||to>=g->node_count) return UMI_STATUS_INVALID_ARGUMENT; if(g->edge_count>=UMI_UI_REACTIVE_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED; g->from_index[g->edge_count]=from;g->to_index[g->edge_count]=to;g->edge_count++;return UMI_STATUS_OK; }
