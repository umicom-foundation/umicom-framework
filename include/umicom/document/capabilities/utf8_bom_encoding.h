/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/utf8_bom_encoding.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Utf8 Bom Encoding document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UTF8_BOM_ENCODING_H
#define UMICOM_DOCUMENT_CAPABILITIES_UTF8_BOM_ENCODING_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UTF8_BOM_ENCODING "umicom.document.encoding.utf8_bom_encoding"

/**
 * Provide the document capability utf8 bom encoding operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf8_bom_encoding(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UTF8_BOM_ENCODING_H */
