/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/binding_graph.c
 *
 * PURPOSE:
 *   Maintain an acyclic visual binding graph with cycle detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/binding_graph.h"
#include <string.h>
/*
 * Initialise visual designer binding graph from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_binding_graph_init(UmiRadBindingGraph *g){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(g,0,sizeof *g);return UMI_STATUS_OK;}
/* Provide the reaches operation used by this module and its client applications. */
static int reaches(const UmiRadBindingGraph *g,const char *from,const char *target,unsigned depth){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(from,target)==0)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(depth>=UMI_RAD_MAX_BINDINGS)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->wires[i].source_node_id,from)==0&&reaches(g,g->wires[i].target_node_id,target,depth+1U))return 1;return 0;}
/*
 * Provide the visual designer binding graph has cycle operation used by this module and its client
 * applications.
 */
int umi_rad_binding_graph_has_cycle(const UmiRadBindingGraph *g){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(reaches(g,g->wires[i].target_node_id,g->wires[i].source_node_id,0U))return 1;return 0;}
/* Add visual designer binding graph only after its inputs and available capacity have been checked. */
UmiStatus umi_rad_binding_graph_add(UmiRadBindingGraph *g,const UmiRadBindingWire *w){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||w==NULL||!umi_rad_binding_wire_is_valid(w))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->count>=UMI_RAD_MAX_BINDINGS)return UMI_STATUS_CAPACITY_EXCEEDED;g->wires[g->count++]=*w;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_rad_binding_graph_has_cycle(g)){g->count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
