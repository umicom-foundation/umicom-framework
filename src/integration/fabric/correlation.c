/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/correlation.c
 *
 * PURPOSE:
 *   Validate request/reply and causation relationships without coupling callers to a transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/correlation.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_correlation_set_causation(UmiFabricMessageHeader *header,const char *causation_id) { if(header==NULL)return UMI_STATUS_INVALID_ARGUMENT; return umi_fabric_copy_text(header->causation_id,sizeof(header->causation_id),causation_id); }
bool umi_fabric_correlation_matches(const UmiFabricMessageHeader *left,const UmiFabricMessageHeader *right) { return left!=NULL && right!=NULL && left->correlation_id[0]!='\0' && strcmp(left->correlation_id,right->correlation_id)==0; }
