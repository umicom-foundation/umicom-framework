/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_hierarchy.c
 *
 * PURPOSE:
 *   Maintain an acyclic parent-child hierarchy for portfolio risk aggregation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/portfolio_hierarchy.h"

#include <string.h>
static const UmiEnterprisePortfolioHierarchyNode *find_node(const UmiEnterprisePortfolioHierarchy *h,const char *id){ size_t i; for(i=0U;i<h->count;++i)if(strcmp(h->nodes[i].node_id,id)==0)return &h->nodes[i]; return NULL; }
void umi_enterprise_portfolio_hierarchy_init(UmiEnterprisePortfolioHierarchy *h){ if(h!=NULL)memset(h,0,sizeof *h); }
UmiStatus umi_enterprise_portfolio_hierarchy_add(UmiEnterprisePortfolioHierarchy *h,const UmiEnterprisePortfolioHierarchyNode *n){ const UmiEnterprisePortfolioHierarchyNode *p; unsigned depth=0U; if(h==NULL||n==NULL||n->node_id[0]=='\0'||!umi_quant_number_valid(n->weight)||n->weight<0.0)return UMI_STATUS_INVALID_ARGUMENT; if(find_node(h,n->node_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS; p=n; while(p->parent_id[0]!='\0'){ if(strcmp(p->parent_id,n->node_id)==0)return UMI_STATUS_INVALID_STATE; p=find_node(h,p->parent_id); if(p==NULL)break; if(++depth>UMI_ENTERPRISE_MAX_HIERARCHY_NODES)return UMI_STATUS_INVALID_STATE; } if(h->count>=UMI_ENTERPRISE_MAX_HIERARCHY_NODES)return UMI_STATUS_CAPACITY_EXCEEDED; h->nodes[h->count++]=*n; return UMI_STATUS_OK; }
