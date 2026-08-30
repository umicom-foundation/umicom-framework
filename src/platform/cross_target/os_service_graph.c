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
static int idx(const UmiCtOsServiceGraph*g,const char*id){size_t i;for(i=0U;i<g->catalogue.count;++i)if(strcmp(g->catalogue.items[i].service_id,id)==0)return (int)i;return -1;}
void umi_ct_os_service_graph_init(UmiCtOsServiceGraph*g){if(g!=NULL)memset(g,0,sizeof(*g));}
UmiStatus umi_ct_os_service_graph_add(UmiCtOsServiceGraph*g,const UmiCtOsServiceDescriptor*s){return g==NULL?UMI_STATUS_INVALID_ARGUMENT:umi_ct_os_service_catalogue_add(&g->catalogue,s);}
UmiStatus umi_ct_os_service_graph_link(UmiCtOsServiceGraph*g,const char*s,const char*d){int a,b;if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT;a=idx(g,s);b=idx(g,d);if(a<0||b<0)return UMI_STATUS_NOT_FOUND;if(a==b)return UMI_STATUS_INVALID_STATE;g->edges[(size_t)a][(size_t)b]=true;return UMI_STATUS_OK;}
UmiStatus umi_ct_os_service_graph_order(const UmiCtOsServiceGraph*g,size_t*out,size_t cap,size_t*cnt){size_t n,i,j,k=0;unsigned indeg[UMI_CT_MAX_ITEMS]={0};bool done[UMI_CT_MAX_ITEMS]={0};if(g==NULL||out==NULL||cnt==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=g->catalogue.count;if(cap<n)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0;i<n;++i)for(j=0;j<n;++j)if(g->edges[i][j])indeg[i]++;while(k<n){size_t pick=n;for(i=0;i<n;++i)if(!done[i]&&indeg[i]==0U){pick=i;break;}if(pick==n)return UMI_STATUS_INVALID_STATE;done[pick]=true;out[k++]=pick;for(i=0;i<n;++i)if(g->edges[i][pick]&&indeg[i]>0U)indeg[i]--;}*cnt=k;return UMI_STATUS_OK;}
