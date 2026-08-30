/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/ui_document_model.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Ui Document Model document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_UI_DOCUMENT_MODEL_H
#define UMICOM_DOCUMENT_CAPABILITIES_UI_DOCUMENT_MODEL_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_UI_DOCUMENT_MODEL "umicom.document.integration.ui_document_model"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_ui_document_model(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_UI_DOCUMENT_MODEL_H */
