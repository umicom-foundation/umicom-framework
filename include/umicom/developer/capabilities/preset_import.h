/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/preset_import.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Preset Import developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H
#define UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PRESET_IMPORT "umicom.developer.configuration.preset_import"

/**
 * Provide the developer capability preset import operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_preset_import(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H */
