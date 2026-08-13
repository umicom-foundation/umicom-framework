/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/encoding_detection.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Encoding Detection document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ENCODING_DETECTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_ENCODING_DETECTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ENCODING_DETECTION "umicom.document.encoding.encoding_detection"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_encoding_detection(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ENCODING_DETECTION_H */
