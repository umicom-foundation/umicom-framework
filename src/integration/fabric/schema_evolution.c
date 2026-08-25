/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_evolution.c
 *
 * PURPOSE:
 *   Represent allowed schema-version transitions and reject cyclic evolution graphs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_evolution.h"
#include <string.h>
#include <limits.h>


static size_t evo_collect(const UmiFabricSchemaEvolution *g,char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY]){size_t i,n=0U,j;for(i=0U;i<g->count;++i){const char *vals[2]={g->edges[i].from_id,g->edges[i].to_id};size_t k;for(k=0U;k<2U;++k){bool seen=false;for(j=0U;j<n;++j)if(strcmp(ids[j],vals[k])==0){seen=true;break;}if(!seen&&n<UMI_FABRIC_MAX_ITEMS){(void)umi_fabric_copy_text(ids[n],UMI_FABRIC_ID_CAPACITY,vals[k]);n++;}}}return n;}
static bool evo_dfs(const UmiFabricSchemaEvolution *g,char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY],size_t n,size_t idx,unsigned char *mark){size_t e,j;mark[idx]=1U;for(e=0U;e<g->count;++e)if(strcmp(g->edges[e].from_id,ids[idx])==0){for(j=0U;j<n;++j)if(strcmp(g->edges[e].to_id,ids[j])==0){if(mark[j]==1U)return true;if(mark[j]==0U&&evo_dfs(g,ids,n,j,mark))return true;}}mark[idx]=2U;return false;}
void umi_fabric_schema_evolution_init(UmiFabricSchemaEvolution *graph){if(graph!=NULL)(void)memset(graph,0,sizeof(*graph));}
UmiStatus umi_fabric_schema_evolution_add(UmiFabricSchemaEvolution *graph,const char *from_id,const char *to_id,bool breaking){UmiStatus s;if(graph==NULL||from_id==NULL||to_id==NULL||strcmp(from_id,to_id)==0)return UMI_STATUS_INVALID_ARGUMENT;if(graph->count>=UMI_FABRIC_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_fabric_copy_text(graph->edges[graph->count].from_id,sizeof(graph->edges[graph->count].from_id),from_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(graph->edges[graph->count].to_id,sizeof(graph->edges[graph->count].to_id),to_id);if(s!=UMI_STATUS_OK)return s;graph->edges[graph->count].breaking=breaking;graph->count++;return umi_fabric_schema_evolution_has_cycle(graph)?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;}
bool umi_fabric_schema_evolution_has_cycle(const UmiFabricSchemaEvolution *graph){char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY]={{0}};unsigned char mark[UMI_FABRIC_MAX_ITEMS]={0};size_t n,i;if(graph==NULL)return false;n=evo_collect(graph,ids);for(i=0U;i<n;++i)if(mark[i]==0U&&evo_dfs(graph,ids,n,i,mark))return true;return false;}
