/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/permission_preservation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Permission Preservation document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_PERMISSION_PRESERVATION_H
#define UMICOM_DOCUMENT_CAPABILITIES_PERMISSION_PRESERVATION_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_PERMISSION_PRESERVATION "umicom.document.persistence.permission_preservation"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_permission_preservation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_PERMISSION_PRESERVATION_H */
