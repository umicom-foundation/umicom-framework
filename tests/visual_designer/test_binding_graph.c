/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_binding_graph.c
 *
 * PURPOSE:
 *   Validate maintain an acyclic visual binding graph with cycle detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/binding_graph.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadBindingGraph g;UmiRadBindingWire a,b;CHECK(umi_rad_binding_graph_init(&g)==UMI_STATUS_OK);CHECK(umi_rad_binding_wire_init(&a)==UMI_STATUS_OK);CHECK(umi_rad_copy_text(a.source_node_id,sizeof a.source_node_id,"a")==UMI_STATUS_OK);CHECK(umi_rad_copy_text(a.target_node_id,sizeof a.target_node_id,"b")==UMI_STATUS_OK);CHECK(umi_rad_binding_graph_add(&g,&a)==UMI_STATUS_OK);b=a;CHECK(umi_rad_copy_text(b.wire_id,sizeof b.wire_id,"w2")==UMI_STATUS_OK);CHECK(umi_rad_copy_text(b.source_node_id,sizeof b.source_node_id,"b")==UMI_STATUS_OK);CHECK(umi_rad_copy_text(b.target_node_id,sizeof b.target_node_id,"a")==UMI_STATUS_OK);CHECK(umi_rad_binding_graph_add(&g,&b)==UMI_STATUS_INVALID_STATE);return 0;}
