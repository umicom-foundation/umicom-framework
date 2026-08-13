/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_history.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document History document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_HISTORY "umicom.document.navigation.document_history"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_history(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_HISTORY_H */
