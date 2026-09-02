/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_compatibility.c
 *
 * PURPOSE:
 *   Classify producer/consumer schema compatibility using semantic version and declared compatibility evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_compatibility.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric schema compatibility classify operation used by this module and its
 * client applications.
 */
UmiFabricSchemaCompatibility umi_fabric_schema_compatibility_classify(const UmiFabricSchemaDescriptor *producer,const UmiFabricSchemaDescriptor *consumer){int cmp;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(producer==NULL||consumer==NULL)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(producer->name,consumer->name)!=0)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(producer->version.major!=consumer->version.major)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;cmp=umi_fabric_version_compare(producer->version,consumer->version);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cmp==0&&producer->fingerprint==consumer->fingerprint)return UMI_FABRIC_SCHEMA_EXACT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cmp>0&&producer->backward_compatible)return UMI_FABRIC_SCHEMA_BACKWARD;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cmp<0&&producer->forward_compatible)return UMI_FABRIC_SCHEMA_FORWARD;return UMI_FABRIC_SCHEMA_INCOMPATIBLE;}
