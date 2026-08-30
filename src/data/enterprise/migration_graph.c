/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_graph.c
 *
 * PURPOSE:
 *   Model migration prerequisites as a bounded DAG and detect dependency cycles before execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_graph.h"
#include <string.h>

/* Graph reset supports migration-plan preview and rebuild. */ void umi_data_migration_graph_init(UmiDataMigrationGraph *graph){if(graph!=NULL)(void)memset(graph,0,sizeof(*graph));}
static int step_index(const UmiDataMigrationGraph *g,const char *id){size_t i;for(i=0;i<g->step_count;++i)if(strcmp(g->steps[i].step_id,id)==0)return (int)i;return -1;}
/* Steps are unique by stable migration identifier. */ UmiStatus umi_data_migration_graph_add_step(UmiDataMigrationGraph *graph,const UmiDataMigrationStep *step){size_t i;if(graph==NULL||step==NULL||umi_data_migration_step_validate(step)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<graph->step_count;++i)if(strcmp(graph->steps[i].step_id,step->step_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(graph->step_count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;graph->steps[graph->step_count++]=*step;return UMI_STATUS_OK;}
/* Edges require existing endpoints so validation never depends on deferred name resolution. */ UmiStatus umi_data_migration_graph_add_edge(UmiDataMigrationGraph *graph,const char *from_step,const char *to_step){UmiDataMigrationEdge *e;if(graph==NULL||from_step==NULL||to_step==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(step_index(graph,from_step)<0||step_index(graph,to_step)<0)return UMI_STATUS_NOT_FOUND;if(strcmp(from_step,to_step)==0)return UMI_STATUS_INVALID_ARGUMENT;if(graph->edge_count>=UMI_DATA_ENTERPRISE_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;e=&graph->edges[graph->edge_count++];(void)umi_data_enterprise_copy_text(e->from_step,sizeof(e->from_step),from_step);(void)umi_data_enterprise_copy_text(e->to_step,sizeof(e->to_step),to_step);return UMI_STATUS_OK;}
static bool visit(const UmiDataMigrationGraph *g,int index,unsigned char *marks){size_t i;if(marks[index]==1U)return true;if(marks[index]==2U)return false;marks[index]=1U;for(i=0;i<g->edge_count;++i)if(strcmp(g->edges[i].from_step,g->steps[index].step_id)==0){int next=step_index(g,g->edges[i].to_step);if(next>=0&&visit(g,next,marks))return true;}marks[index]=2U;return false;}
/* Cycle detection prevents partially executable migration sets from reaching the backend. */ bool umi_data_migration_graph_has_cycle(const UmiDataMigrationGraph *graph){unsigned char marks[UMI_DATA_ENTERPRISE_MAX_ITEMS]={0};size_t i;if(graph==NULL)return true;for(i=0;i<graph->step_count;++i)if(visit(graph,(int)i,marks))return true;return false;}
