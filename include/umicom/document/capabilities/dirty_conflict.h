/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/dirty_conflict.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Dirty Conflict document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H
#define UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DIRTY_CONFLICT "umicom.document.conflict.dirty_conflict"

/**
 * Provide the document capability dirty conflict operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_dirty_conflict(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H */
