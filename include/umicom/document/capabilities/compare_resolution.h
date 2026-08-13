/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/compare_resolution.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Compare Resolution document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_COMPARE_RESOLUTION "umicom.document.conflict.compare_resolution"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_compare_resolution(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H */
