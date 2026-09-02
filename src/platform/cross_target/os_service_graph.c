/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/os_service_graph.c
 *
 * PURPOSE:
 *   Build an acyclic Umicom OS service dependency graph and produce deterministic topological boot order.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/os_service_graph.h"

#include <string.h>
/* Provide the idx operation used by this module and its client applications. */
static int idx(const UmiCtOsServiceGraph*g,const char*id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->catalogue.count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->catalogue.items[i].service_id,id)==0)return (int)i;return -1;}
/*
 * Initialise ct os service graph from caller-provided values so later operations receive a
 * known state.
 */
void umi_ct_os_service_graph_init(UmiCtOsServiceGraph*g){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL)memset(g,0,sizeof(*g));}
/* Add ct os service graph only after its inputs and available capacity have been checked. */
UmiStatus umi_ct_os_service_graph_add(UmiCtOsServiceGraph*g,const UmiCtOsServiceDescriptor*s){return g==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_ct_os_service_catalogue_add(&g->catalogue,s);}
/*
 * Provide the ct os service graph link operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_os_service_graph_link(UmiCtOsServiceGraph*g,const char*s,const char*d){int a,b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT;a=idx(g,s);b=idx(g,d);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a<0||b<0)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==b)return UMI_STATUS_INVALID_STATE;g->edges[(size_t)a][(size_t)b]=true;return UMI_STATUS_OK;}
/*
 * Provide the ct os service graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_os_service_graph_order(const UmiCtOsServiceGraph*g,size_t*out,size_t cap,size_t*cnt){size_t n,i,j,k=0;unsigned indeg[UMI_CT_MAX_ITEMS]={0};bool done[UMI_CT_MAX_ITEMS]={0};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||out==NULL||cnt==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=g->catalogue.count;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cap<n)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;++i)/* Visit each bounded item once so every record receives the same rule. */ for(j=0;j<n;++j)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edges[i][j])indeg[i]++;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(k<n){size_t pick=n;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!done[i]&&indeg[i]==0U){pick=i;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pick==n)return UMI_STATUS_INVALID_STATE;done[pick]=true;out[k++]=pick;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edges[i][pick]&&indeg[i]>0U)indeg[i]--;}*cnt=k;return UMI_STATUS_OK;}
