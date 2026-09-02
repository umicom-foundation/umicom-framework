/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/compare_resolution.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Compare Resolution document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_COMPARE_RESOLUTION "umicom.document.conflict.compare_resolution"

/**
 * Provide the document capability compare resolution operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_compare_resolution(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_COMPARE_RESOLUTION_H */
