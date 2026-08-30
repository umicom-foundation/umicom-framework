/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/utf16le_encoding.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Utf16le Encoding document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UTF16LE_ENCODING_H
#define UMICOM_DOCUMENT_CAPABILITIES_UTF16LE_ENCODING_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UTF16LE_ENCODING "umicom.document.encoding.utf16le_encoding"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf16le_encoding(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UTF16LE_ENCODING_H */
