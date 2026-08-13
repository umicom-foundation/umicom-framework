/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/dirty_conflict.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Dirty Conflict document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H
#define UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DIRTY_CONFLICT "umicom.document.conflict.dirty_conflict"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_dirty_conflict(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DIRTY_CONFLICT_H */
