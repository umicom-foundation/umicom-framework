/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/diagnostic_export.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Diagnostic Export developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_EXPORT_H
#define UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_EXPORT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DIAGNOSTIC_EXPORT "umicom.developer.diagnostics.diagnostic_export"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_diagnostic_export(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DIAGNOSTIC_EXPORT_H */
