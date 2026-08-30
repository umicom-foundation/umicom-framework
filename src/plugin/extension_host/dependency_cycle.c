/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/dependency_cycle.c
 *
 * PURPOSE:
 *   Detect cycles in extension dependency topology before activation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/dependency_cycle.h"
#include <string.h>
static int umi_plugin_extension_host_dependency_cycle_visit(const UmiPluginExtensionHostDependencyCycle *g,size_t node,uint8_t *state) { size_t i; state[node]=1U; for(i=0U;i<g->node_count;++i) if(g->edges[node][i]!=0U) { if(state[i]==1U) return 1; if(state[i]==0U&&umi_plugin_extension_host_dependency_cycle_visit(g,i,state)) return 1; } state[node]=2U; return 0; }
void umi_plugin_extension_host_dependency_cycle_init(UmiPluginExtensionHostDependencyCycle *graph,size_t node_count) { if(graph!=NULL) { memset(graph,0,sizeof(*graph)); graph->node_count=node_count>UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY?UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY:node_count; } }
UmiStatus umi_plugin_extension_host_dependency_cycle_add_edge(UmiPluginExtensionHostDependencyCycle *graph,size_t from,size_t to) { if(graph==NULL||from>=graph->node_count||to>=graph->node_count||from==to) return UMI_STATUS_INVALID_ARGUMENT; graph->edges[from][to]=1U; return UMI_STATUS_OK; }
int umi_plugin_extension_host_dependency_cycle_has_cycle(const UmiPluginExtensionHostDependencyCycle *graph) { uint8_t state[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]={0}; size_t i; if(graph==NULL) return 0; for(i=0U;i<graph->node_count;++i) if(state[i]==0U&&umi_plugin_extension_host_dependency_cycle_visit(graph,i,state)) return 1; return 0; }
