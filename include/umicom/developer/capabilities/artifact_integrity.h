/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/artifact_integrity.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Artifact Integrity developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INTEGRITY_H
#define UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INTEGRITY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ARTIFACT_INTEGRITY "umicom.developer.security.artifact_integrity"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_artifact_integrity(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INTEGRITY_H */
