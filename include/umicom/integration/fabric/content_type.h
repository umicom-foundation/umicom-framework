/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/content_type.h
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
#ifndef UMICOM_INTEGRATION_FABRIC_CONTENT_TYPE_H
#define UMICOM_INTEGRATION_FABRIC_CONTENT_TYPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiFabricContentKind { UMI_FABRIC_CONTENT_UNKNOWN=0,UMI_FABRIC_CONTENT_JSON=1,UMI_FABRIC_CONTENT_TEXT=2,UMI_FABRIC_CONTENT_BINARY=3,UMI_FABRIC_CONTENT_XML=4 } UmiFabricContentKind;
UmiFabricContentKind umi_fabric_content_type_classify(const char *content_type);
bool umi_fabric_content_type_compatible(UmiFabricContentKind producer,UmiFabricContentKind consumer);

#ifdef __cplusplus
}
#endif
#endif
