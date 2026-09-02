/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/correlation.c
 *
 * PURPOSE:
 *   Validate request/reply and causation relationships without coupling callers to a transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/correlation.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric correlation set causation operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_correlation_set_causation(UmiFabricMessageHeader *header,const char *causation_id) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(header==NULL)return UMI_STATUS_INVALID_ARGUMENT; return umi_fabric_copy_text(header->causation_id,sizeof(header->causation_id),causation_id); }
/*
 * Provide the fabric correlation matches operation used by this module and its client
 * applications.
 */
bool umi_fabric_correlation_matches(const UmiFabricMessageHeader *left,const UmiFabricMessageHeader *right) { return left!=NULL && right!=NULL && left->correlation_id[0]!='\0' && strcmp(left->correlation_id,right->correlation_id)==0; }
