/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/vcs_diff_bridge.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Vcs Diff Bridge document capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_VCS_DIFF_BRIDGE_H
#define UMICOM_DOCUMENT_CAPABILITIES_VCS_DIFF_BRIDGE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_VCS_DIFF_BRIDGE "umicom.document.integration.vcs_diff_bridge"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_vcs_diff_bridge(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_VCS_DIFF_BRIDGE_H */
