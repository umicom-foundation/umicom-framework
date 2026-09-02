/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_regeneration_graph.c
 *
 * PURPOSE:
 *   Regression coverage for track dependencies among regeneration units and reject cyclic regeneration plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/regeneration_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRegenerationGraph g; umi_helix_regeneration_graph_init(&g); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_regeneration_graph_add(&g,"a")!=UMI_STATUS_OK||umi_helix_regeneration_graph_add(&g,"b")!=UMI_STATUS_OK){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_regeneration_graph_depend(&g,0,1)!=UMI_STATUS_OK){ return 2; } size_t o[2],n=0; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_regeneration_graph_order(&g,o,2,&n)!=UMI_STATUS_OK||n!=2||o[0]!=0||o[1]!=1){ return 3; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_regeneration_graph_depend(&g,1,0)!=UMI_STATUS_OK||!umi_helix_regeneration_graph_has_cycle(&g)){ return 4; } return 0; }
