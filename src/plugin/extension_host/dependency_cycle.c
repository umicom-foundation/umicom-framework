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
/*
 * Provide the plugin extension host dependency cycle visit operation used by this module
 * and its client applications.
 */
static int umi_plugin_extension_host_dependency_cycle_visit(const UmiPluginExtensionHostDependencyCycle *g,size_t node,uint8_t *state) { size_t i; state[node]=1U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->node_count;++i) /* Apply this branch only when its contract condition is satisfied. */ if(g->edges[node][i]!=0U) { /* Apply this branch only when its contract condition is satisfied. */ if(state[i]==1U) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(state[i]==0U&&umi_plugin_extension_host_dependency_cycle_visit(g,i,state)) return 1; } state[node]=2U; return 0; }
/*
 * Initialise plugin extension host dependency cycle from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_dependency_cycle_init(UmiPluginExtensionHostDependencyCycle *graph,size_t node_count) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph!=NULL) { memset(graph,0,sizeof(*graph)); graph->node_count=node_count>UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY?UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY:node_count; } }
/*
 * Provide the plugin extension host dependency cycle add edge operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_dependency_cycle_add_edge(UmiPluginExtensionHostDependencyCycle *graph,size_t from,size_t to) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||from>=graph->node_count||to>=graph->node_count||from==to) return UMI_STATUS_INVALID_ARGUMENT; graph->edges[from][to]=1U; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host dependency cycle has cycle operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_cycle_has_cycle(const UmiPluginExtensionHostDependencyCycle *graph) { uint8_t state[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]={0}; size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL) return 0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->node_count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(state[i]==0U&&umi_plugin_extension_host_dependency_cycle_visit(graph,i,state)) return 1; return 0; }
