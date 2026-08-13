/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/workspace_trust.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Workspace Trust document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_WORKSPACE_TRUST_H
#define UMICOM_DOCUMENT_CAPABILITIES_WORKSPACE_TRUST_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_WORKSPACE_TRUST "umicom.document.security.workspace_trust"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_workspace_trust(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_WORKSPACE_TRUST_H */
