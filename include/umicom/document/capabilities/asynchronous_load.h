/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/asynchronous_load.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Asynchronous Load document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_LOAD_H
#define UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_LOAD_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ASYNCHRONOUS_LOAD "umicom.document.performance.asynchronous_load"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_asynchronous_load(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ASYNCHRONOUS_LOAD_H */
