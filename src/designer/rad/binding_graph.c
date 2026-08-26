/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/binding_graph.c
 *
 * PURPOSE:
 *   Maintain an acyclic visual binding graph with cycle detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/binding_graph.h"
#include <string.h>
UmiStatus umi_rad_binding_graph_init(UmiRadBindingGraph *g){if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(g,0,sizeof *g);return UMI_STATUS_OK;}
static int reaches(const UmiRadBindingGraph *g,const char *from,const char *target,unsigned depth){size_t i;if(strcmp(from,target)==0)return 1;if(depth>=UMI_RAD_MAX_BINDINGS)return 0;for(i=0U;i<g->count;i++)if(strcmp(g->wires[i].source_node_id,from)==0&&reaches(g,g->wires[i].target_node_id,target,depth+1U))return 1;return 0;}
int umi_rad_binding_graph_has_cycle(const UmiRadBindingGraph *g){size_t i;if(g==NULL)return 0;for(i=0U;i<g->count;i++)if(reaches(g,g->wires[i].target_node_id,g->wires[i].source_node_id,0U))return 1;return 0;}
UmiStatus umi_rad_binding_graph_add(UmiRadBindingGraph *g,const UmiRadBindingWire *w){if(g==NULL||w==NULL||!umi_rad_binding_wire_is_valid(w))return UMI_STATUS_INVALID_ARGUMENT;if(g->count>=UMI_RAD_MAX_BINDINGS)return UMI_STATUS_CAPACITY_EXCEEDED;g->wires[g->count++]=*w;if(umi_rad_binding_graph_has_cycle(g)){g->count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
