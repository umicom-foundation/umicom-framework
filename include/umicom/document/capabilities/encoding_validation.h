/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/encoding_validation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Encoding Validation document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ENCODING_VALIDATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_ENCODING_VALIDATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ENCODING_VALIDATION "umicom.document.encoding.encoding_validation"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_encoding_validation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ENCODING_VALIDATION_H */
