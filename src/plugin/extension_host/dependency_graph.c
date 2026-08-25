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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/dependency_graph.h"
#include <string.h>

static void umi_plugin_extension_host_dependency_graph_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_dependency_graph_init(UmiPluginExtensionHostDependencyGraph *graph) { if(graph!=NULL) memset(graph,0,sizeof(*graph)); }
UmiStatus umi_plugin_extension_host_dependency_graph_add(UmiPluginExtensionHostDependencyGraph *graph,const char *from,const char *to,int optional) { size_t i; if(graph==NULL||!umi_plugin_extension_host_text_valid(from,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||!umi_plugin_extension_host_text_valid(to,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||strcmp(from,to)==0) return UMI_STATUS_INVALID_ARGUMENT; if(graph->count>=UMI_PLUGIN_EXTENSION_HOST_LARGE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; for(i=0U;i<graph->count;++i) if(strcmp(graph->edges[i].from,from)==0&&strcmp(graph->edges[i].to,to)==0) return UMI_STATUS_ALREADY_EXISTS; i=graph->count++; umi_plugin_extension_host_dependency_graph_copy(graph->edges[i].from,sizeof(graph->edges[i].from),from); umi_plugin_extension_host_dependency_graph_copy(graph->edges[i].to,sizeof(graph->edges[i].to),to); graph->edges[i].optional=optional?1:0; ++graph->revision; return UMI_STATUS_OK; }
int umi_plugin_extension_host_dependency_graph_depends_on(const UmiPluginExtensionHostDependencyGraph *graph,const char *from,const char *to) { size_t i; if(graph==NULL||from==NULL||to==NULL) return 0; for(i=0U;i<graph->count;++i) if(strcmp(graph->edges[i].from,from)==0&&strcmp(graph->edges[i].to,to)==0) return 1; return 0; }
size_t umi_plugin_extension_host_dependency_graph_out_degree(const UmiPluginExtensionHostDependencyGraph *graph,const char *from) { size_t i,n=0U; if(graph==NULL||from==NULL) return 0U; for(i=0U;i<graph->count;++i) if(strcmp(graph->edges[i].from,from)==0) ++n; return n; }
