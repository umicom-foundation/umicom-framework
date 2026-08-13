/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/health_snapshot.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Health Snapshot developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_HEALTH_SNAPSHOT_H
#define UMICOM_DEVELOPER_CAPABILITIES_HEALTH_SNAPSHOT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_HEALTH_SNAPSHOT "umicom.developer.diagnostics.health_snapshot"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_health_snapshot(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_HEALTH_SNAPSHOT_H */
