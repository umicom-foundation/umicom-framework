/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/save_as.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Save As document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_H
#define UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_SAVE_AS "umicom.document.persistence.save_as"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_as(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_SAVE_AS_H */
