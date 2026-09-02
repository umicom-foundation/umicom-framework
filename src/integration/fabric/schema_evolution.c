/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_evolution.c
 *
 * PURPOSE:
 *   Represent allowed schema-version transitions and reject cyclic evolution graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_evolution.h"
#include <string.h>
#include <limits.h>


/* Provide the evo collect operation used by this module and its client applications. */
static size_t evo_collect(const UmiFabricSchemaEvolution *g,char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY]){size_t i,n=0U,j;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->count;++i){const char *vals[2]={g->edges[i].from_id,g->edges[i].to_id};size_t k;/* Visit each bounded item once so every record receives the same rule. */ for(k=0U;k<2U;++k){bool seen=false;/* Visit each bounded item once so every record receives the same rule. */ for(j=0U;j<n;++j)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(ids[j],vals[k])==0){seen=true;break;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!seen&&n<UMI_FABRIC_MAX_ITEMS){(void)umi_fabric_copy_text(ids[n],UMI_FABRIC_ID_CAPACITY,vals[k]);n++;}}}return n;}
/* Provide the evo dfs operation used by this module and its client applications. */
static bool evo_dfs(const UmiFabricSchemaEvolution *g,char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY],size_t n,size_t idx,unsigned char *mark){size_t e,j;mark[idx]=1U;/* Visit each bounded item once so every record receives the same rule. */ for(e=0U;e<g->count;++e)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->edges[e].from_id,ids[idx])==0){/* Visit each bounded item once so every record receives the same rule. */ for(j=0U;j<n;++j)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->edges[e].to_id,ids[j])==0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(mark[j]==1U)return true;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(mark[j]==0U&&evo_dfs(g,ids,n,j,mark))return true;}}mark[idx]=2U;return false;}
/*
 * Initialise fabric schema evolution from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_schema_evolution_init(UmiFabricSchemaEvolution *graph){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph!=NULL)(void)memset(graph,0,sizeof(*graph));}
/*
 * Add fabric schema evolution only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_schema_evolution_add(UmiFabricSchemaEvolution *graph,const char *from_id,const char *to_id,bool breaking){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||from_id==NULL||to_id==NULL||strcmp(from_id,to_id)==0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph->count>=UMI_FABRIC_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_fabric_copy_text(graph->edges[graph->count].from_id,sizeof(graph->edges[graph->count].from_id),from_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(graph->edges[graph->count].to_id,sizeof(graph->edges[graph->count].to_id),to_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;graph->edges[graph->count].breaking=breaking;graph->count++;return umi_fabric_schema_evolution_has_cycle(graph)?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;}
/*
 * Provide the fabric schema evolution has cycle operation used by this module and its
 * client applications.
 */
bool umi_fabric_schema_evolution_has_cycle(const UmiFabricSchemaEvolution *graph){char ids[UMI_FABRIC_MAX_ITEMS][UMI_FABRIC_ID_CAPACITY]={{0}};unsigned char mark[UMI_FABRIC_MAX_ITEMS]={0};size_t n,i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL)return false;n=evo_collect(graph,ids);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<n;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(mark[i]==0U&&evo_dfs(graph,ids,n,i,mark))return true;return false;}
