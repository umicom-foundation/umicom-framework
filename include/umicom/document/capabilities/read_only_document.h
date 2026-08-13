/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/read_only_document.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Read Only Document document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_READ_ONLY_DOCUMENT_H
#define UMICOM_DOCUMENT_CAPABILITIES_READ_ONLY_DOCUMENT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_READ_ONLY_DOCUMENT "umicom.document.lifecycle.read_only_document"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_read_only_document(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_READ_ONLY_DOCUMENT_H */
