/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_compatibility.c
 *
 * PURPOSE:
 *   Classify producer/consumer schema compatibility using semantic version and declared compatibility evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_compatibility.h"
#include <string.h>
#include <limits.h>


UmiFabricSchemaCompatibility umi_fabric_schema_compatibility_classify(const UmiFabricSchemaDescriptor *producer,const UmiFabricSchemaDescriptor *consumer){int cmp;if(producer==NULL||consumer==NULL)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;if(strcmp(producer->name,consumer->name)!=0)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;if(producer->version.major!=consumer->version.major)return UMI_FABRIC_SCHEMA_INCOMPATIBLE;cmp=umi_fabric_version_compare(producer->version,consumer->version);if(cmp==0&&producer->fingerprint==consumer->fingerprint)return UMI_FABRIC_SCHEMA_EXACT;if(cmp>0&&producer->backward_compatible)return UMI_FABRIC_SCHEMA_BACKWARD;if(cmp<0&&producer->forward_compatible)return UMI_FABRIC_SCHEMA_FORWARD;return UMI_FABRIC_SCHEMA_INCOMPATIBLE;}
