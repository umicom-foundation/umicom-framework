/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/autosave.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Autosave document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_AUTOSAVE_H
#define UMICOM_DOCUMENT_CAPABILITIES_AUTOSAVE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_AUTOSAVE "umicom.document.recovery.autosave"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_autosave(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_AUTOSAVE_H */
