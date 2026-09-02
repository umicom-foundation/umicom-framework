/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/encoding_conversion.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Encoding Conversion document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ENCODING_CONVERSION_H
#define UMICOM_DOCUMENT_CAPABILITIES_ENCODING_CONVERSION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ENCODING_CONVERSION "umicom.document.encoding.encoding_conversion"

/**
 * Provide the document capability encoding conversion operation used by this module and
 * its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_encoding_conversion(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ENCODING_CONVERSION_H */
