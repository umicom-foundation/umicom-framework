/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/artifact_indexing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Artifact Indexing developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INDEXING_H
#define UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INDEXING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ARTIFACT_INDEXING "umicom.developer.build.artifact_indexing"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_artifact_indexing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ARTIFACT_INDEXING_H */
