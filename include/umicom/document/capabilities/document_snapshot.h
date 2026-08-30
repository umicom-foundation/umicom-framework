/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_snapshot.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document Snapshot document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_SNAPSHOT_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_SNAPSHOT_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_SNAPSHOT "umicom.document.recovery.document_snapshot"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_snapshot(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_SNAPSHOT_H */
