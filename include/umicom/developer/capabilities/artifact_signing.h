/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/artifact_signing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Artifact Signing developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_SIGNING_H
#define UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_SIGNING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ARTIFACT_SIGNING "umicom.developer.delivery.artifact_signing"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_artifact_signing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_SIGNING_H */
