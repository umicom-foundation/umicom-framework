/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/machine_reports.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Machine Reports developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MACHINE_REPORTS_H
#define UMICOM_DEVELOPER_CAPABILITIES_MACHINE_REPORTS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MACHINE_REPORTS "umicom.developer.testing.machine_reports"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_machine_reports(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MACHINE_REPORTS_H */
