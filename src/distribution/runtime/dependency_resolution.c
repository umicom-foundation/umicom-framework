/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_resolution.c
 *
 * PURPOSE:
 *   bounded dependency graph with cycle detection and topological readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_resolution.h"

static bool reach(const UmiDrDependencyResolution*g,size_t at,size_t target,bool *seen){size_t i;if(at==target)return true;if(seen[at])return false;seen[at]=true;for(i=0U;i<g->edge_count;++i)if(g->edges[i].from==at&&reach(g,g->edges[i].to,target,seen))return true;return false;}
void umi_dr_dependency_resolution_init(UmiDrDependencyResolution*g,size_t n){if(g!=NULL){*g=(UmiDrDependencyResolution){0};g->node_count=n;}}
UmiStatus umi_dr_dependency_resolution_add(UmiDrDependencyResolution*g,size_t f,size_t t){if(g==NULL||f>=g->node_count||t>=g->node_count||f==t)return UMI_STATUS_INVALID_ARGUMENT;if(g->edge_count>=UMI_DR_MAX_DEPENDENCIES)return UMI_STATUS_CAPACITY_EXCEEDED;g->edges[g->edge_count++]=(UmiDrDependencyEdge){(uint16_t)f,(uint16_t)t};return UMI_STATUS_OK;}
bool umi_dr_dependency_resolution_has_cycle(const UmiDrDependencyResolution*g){size_t i;bool seen[UMI_DR_MAX_ITEMS];if(g==NULL||g->node_count>UMI_DR_MAX_ITEMS)return true;for(i=0U;i<g->edge_count;++i){size_t j;for(j=0U;j<UMI_DR_MAX_ITEMS;++j)seen[j]=false;if(reach(g,g->edges[i].to,g->edges[i].from,seen))return true;}return false;}
