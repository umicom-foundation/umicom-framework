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

/* Provide the reach operation used by this module and its client applications. */
static bool reach(const UmiDrDependencyResolution*g,size_t at,size_t target,bool *seen){size_t i;/* Apply this branch only when its contract condition is satisfied. */ if(at==target)return true;/* Apply this branch only when its contract condition is satisfied. */ if(seen[at])return false;seen[at]=true;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->edge_count;++i)/* Apply this branch only when its contract condition is satisfied. */ if(g->edges[i].from==at&&reach(g,g->edges[i].to,target,seen))return true;return false;}
/*
 * Initialise dr dependency resolution from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_dependency_resolution_init(UmiDrDependencyResolution*g,size_t n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL){*g=(UmiDrDependencyResolution){0};g->node_count=n;}}
/*
 * Add dr dependency resolution only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_dr_dependency_resolution_add(UmiDrDependencyResolution*g,size_t f,size_t t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||f>=g->node_count||t>=g->node_count||f==t)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edge_count>=UMI_DR_MAX_DEPENDENCIES)return UMI_STATUS_CAPACITY_EXCEEDED;g->edges[g->edge_count++]=(UmiDrDependencyEdge){(uint16_t)f,(uint16_t)t};return UMI_STATUS_OK;}
/*
 * Provide the dr dependency resolution has cycle operation used by this module and its
 * client applications.
 */
bool umi_dr_dependency_resolution_has_cycle(const UmiDrDependencyResolution*g){size_t i;bool seen[UMI_DR_MAX_ITEMS];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||g->node_count>UMI_DR_MAX_ITEMS)return true;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->edge_count;++i){size_t j;/* Visit each bounded item once so every record receives the same rule. */ for(j=0U;j<UMI_DR_MAX_ITEMS;++j)seen[j]=false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(reach(g,g->edges[i].to,g->edges[i].from,seen))return true;}return false;}
