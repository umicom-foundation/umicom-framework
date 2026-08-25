/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_regeneration_graph.c
 *
 * PURPOSE:
 *   Regression coverage for track dependencies among regeneration units and reject cyclic regeneration plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/regeneration_graph.h"
int main(void){ UmiHelixRegenerationGraph g; umi_helix_regeneration_graph_init(&g); if(umi_helix_regeneration_graph_add(&g,"a")!=UMI_STATUS_OK||umi_helix_regeneration_graph_add(&g,"b")!=UMI_STATUS_OK){ return 1; } if(umi_helix_regeneration_graph_depend(&g,0,1)!=UMI_STATUS_OK){ return 2; } size_t o[2],n=0; if(umi_helix_regeneration_graph_order(&g,o,2,&n)!=UMI_STATUS_OK||n!=2||o[0]!=0||o[1]!=1){ return 3; } if(umi_helix_regeneration_graph_depend(&g,1,0)!=UMI_STATUS_OK||!umi_helix_regeneration_graph_has_cycle(&g)){ return 4; } return 0; }
