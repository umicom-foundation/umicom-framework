/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/crash_reporting.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Crash Reporting developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CRASH_REPORTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_CRASH_REPORTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CRASH_REPORTING "umicom.developer.diagnostics.crash_reporting"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_crash_reporting(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CRASH_REPORTING_H */
