/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/cold_start.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Cold Start developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_COLD_START_H
#define UMICOM_DEVELOPER_CAPABILITIES_COLD_START_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_COLD_START "umicom.developer.performance.cold_start"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_cold_start(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_COLD_START_H */
