/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/asynchronous_save.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Asynchronous Save document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H
#define UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ASYNCHRONOUS_SAVE "umicom.document.performance.asynchronous_save"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_asynchronous_save(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_SAVE_H */
