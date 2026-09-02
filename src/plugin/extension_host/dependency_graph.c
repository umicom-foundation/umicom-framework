/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/dependency_graph.c
 *
 * PURPOSE:
 *   Maintain bounded extension dependency edges and dependency queries.
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
#include "umicom/plugin/extension_host/dependency_graph.h"
#include <string.h>

/*
 * Copy plugin extension host dependency graph into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_plugin_extension_host_dependency_graph_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

/*
 * Initialise plugin extension host dependency graph from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_dependency_graph_init(UmiPluginExtensionHostDependencyGraph *graph) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph!=NULL) memset(graph,0,sizeof(*graph)); }
/*
 * Add plugin extension host dependency graph only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_dependency_graph_add(UmiPluginExtensionHostDependencyGraph *graph,const char *from,const char *to,int optional) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||!umi_plugin_extension_host_text_valid(from,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||!umi_plugin_extension_host_text_valid(to,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||strcmp(from,to)==0) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph->count>=UMI_PLUGIN_EXTENSION_HOST_LARGE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->edges[i].from,from)==0&&strcmp(graph->edges[i].to,to)==0) return UMI_STATUS_ALREADY_EXISTS; i=graph->count++; umi_plugin_extension_host_dependency_graph_copy(graph->edges[i].from,sizeof(graph->edges[i].from),from); umi_plugin_extension_host_dependency_graph_copy(graph->edges[i].to,sizeof(graph->edges[i].to),to); graph->edges[i].optional=optional?1:0; ++graph->revision; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host dependency graph depends on operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_dependency_graph_depends_on(const UmiPluginExtensionHostDependencyGraph *graph,const char *from,const char *to) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||from==NULL||to==NULL) return 0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->edges[i].from,from)==0&&strcmp(graph->edges[i].to,to)==0) return 1; return 0; }
/*
 * Provide the plugin extension host dependency graph out degree operation used by this
 * module and its client applications.
 */
size_t umi_plugin_extension_host_dependency_graph_out_degree(const UmiPluginExtensionHostDependencyGraph *graph,const char *from) { size_t i,n=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||from==NULL) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->edges[i].from,from)==0) ++n; return n; }
