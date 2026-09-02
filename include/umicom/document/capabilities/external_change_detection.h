/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/external_change_detection.h
 *
 * PURPOSE:
 *   Declare the Framework-owned External Change Detection document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_EXTERNAL_CHANGE_DETECTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_EXTERNAL_CHANGE_DETECTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_EXTERNAL_CHANGE_DETECTION "umicom.document.conflict.external_change_detection"

/**
 * Provide the document capability external change detection operation used by this module
 * and its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_external_change_detection(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_EXTERNAL_CHANGE_DETECTION_H */
