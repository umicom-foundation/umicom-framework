/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/asynchronous_save.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Asynchronous Save document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H
#define UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ASYNCHRONOUS_SAVE "umicom.document.performance.asynchronous_save"

/**
 * Write document capability asynchronous in its stable representation and report capacity
 * or input failures to the caller.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_asynchronous_save(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H */
