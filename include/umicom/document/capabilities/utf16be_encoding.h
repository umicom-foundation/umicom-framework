/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/utf16be_encoding.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Utf16be Encoding document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UTF16BE_ENCODING_H
#define UMICOM_DOCUMENT_CAPABILITIES_UTF16BE_ENCODING_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UTF16BE_ENCODING "umicom.document.encoding.utf16be_encoding"

/**
 * Provide the document capability utf16be encoding operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf16be_encoding(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UTF16BE_ENCODING_H */
