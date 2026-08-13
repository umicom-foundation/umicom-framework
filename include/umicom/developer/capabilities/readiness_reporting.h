/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/readiness_reporting.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Readiness Reporting developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_READINESS_REPORTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_READINESS_REPORTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_READINESS_REPORTING "umicom.developer.diagnostics.readiness_reporting"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_readiness_reporting(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_READINESS_REPORTING_H */
