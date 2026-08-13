/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/orphan_cleanup.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Orphan Cleanup document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_ORPHAN_CLEANUP_H
#define UMICOM_DOCUMENT_CAPABILITIES_ORPHAN_CLEANUP_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_ORPHAN_CLEANUP "umicom.document.recovery.orphan_cleanup"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_orphan_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_ORPHAN_CLEANUP_H */
