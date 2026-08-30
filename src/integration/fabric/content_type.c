/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/content_type.c
 *
 * PURPOSE:
 *   Normalize and classify common integration content types without owning serializers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/content_type.h"
#include <string.h>
#include <limits.h>


UmiFabricContentKind umi_fabric_content_type_classify(const char *content_type){if(content_type==NULL)return UMI_FABRIC_CONTENT_UNKNOWN;if(strncmp(content_type,"application/json",16U)==0)return UMI_FABRIC_CONTENT_JSON;if(strncmp(content_type,"text/",5U)==0)return UMI_FABRIC_CONTENT_TEXT;if(strncmp(content_type,"application/xml",15U)==0||strncmp(content_type,"text/xml",8U)==0)return UMI_FABRIC_CONTENT_XML;if(strncmp(content_type,"application/octet-stream",24U)==0)return UMI_FABRIC_CONTENT_BINARY;return UMI_FABRIC_CONTENT_UNKNOWN;}
bool umi_fabric_content_type_compatible(UmiFabricContentKind producer,UmiFabricContentKind consumer){return producer!=UMI_FABRIC_CONTENT_UNKNOWN && producer==consumer;}
