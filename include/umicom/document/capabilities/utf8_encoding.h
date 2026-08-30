/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/utf8_encoding.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Utf8 Encoding document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UTF8_ENCODING_H
#define UMICOM_DOCUMENT_CAPABILITIES_UTF8_ENCODING_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UTF8_ENCODING "umicom.document.encoding.utf8_encoding"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf8_encoding(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UTF8_ENCODING_H */
