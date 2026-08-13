/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/preset_import.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Preset Import developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H
#define UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PRESET_IMPORT "umicom.developer.configuration.preset_import"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_preset_import(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PRESET_IMPORT_H */
