/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/merge_resolution.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Merge Resolution document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_MERGE_RESOLUTION_H
#define UMICOM_DOCUMENT_CAPABILITIES_MERGE_RESOLUTION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_MERGE_RESOLUTION "umicom.document.conflict.merge_resolution"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_merge_resolution(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_MERGE_RESOLUTION_H */
