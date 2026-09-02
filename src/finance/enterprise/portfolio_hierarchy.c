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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/portfolio_hierarchy.h"

#include <string.h>
/* Provide the find node operation used by this module and its client applications. */
static const UmiEnterprisePortfolioHierarchyNode *find_node(const UmiEnterprisePortfolioHierarchy *h,const char *id){ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<h->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(h->nodes[i].node_id,id)==0)return &h->nodes[i]; return NULL; }
/*
 * Initialise enterprise portfolio hierarchy from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_portfolio_hierarchy_init(UmiEnterprisePortfolioHierarchy *h){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(h!=NULL)memset(h,0,sizeof *h); }
/*
 * Add enterprise portfolio hierarchy only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_enterprise_portfolio_hierarchy_add(UmiEnterprisePortfolioHierarchy *h,const UmiEnterprisePortfolioHierarchyNode *n){ const UmiEnterprisePortfolioHierarchyNode *p; unsigned depth=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||n==NULL||n->node_id[0]=='\0'||!umi_quant_number_valid(n->weight)||n->weight<0.0)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_node(h,n->node_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS; p=n; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(p->parent_id[0]!='\0'){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->parent_id,n->node_id)==0)return UMI_STATUS_INVALID_STATE; p=find_node(h,p->parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)break; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(++depth>UMI_ENTERPRISE_MAX_HIERARCHY_NODES)return UMI_STATUS_INVALID_STATE; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count>=UMI_ENTERPRISE_MAX_HIERARCHY_NODES)return UMI_STATUS_CAPACITY_EXCEEDED; h->nodes[h->count++]=*n; return UMI_STATUS_OK; }
