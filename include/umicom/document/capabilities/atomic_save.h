/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/atomic_save.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Atomic Save document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ATOMIC_SAVE_H
#define UMICOM_DOCUMENT_CAPABILITIES_ATOMIC_SAVE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ATOMIC_SAVE "umicom.document.persistence.atomic_save"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_atomic_save(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ATOMIC_SAVE_H */
