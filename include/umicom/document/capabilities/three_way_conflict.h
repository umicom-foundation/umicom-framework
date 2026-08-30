/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/three_way_conflict.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Three Way Conflict document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_THREE_WAY_CONFLICT_H
#define UMICOM_DOCUMENT_CAPABILITIES_THREE_WAY_CONFLICT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_THREE_WAY_CONFLICT "umicom.document.conflict.three_way_conflict"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_three_way_conflict(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_THREE_WAY_CONFLICT_H */
