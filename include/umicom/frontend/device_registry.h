/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/device_registry.h
 *
 * PURPOSE:
 *   Publish the public device registry contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_DEVICE_REGISTRY_H
#define UMICOM_FRONTEND_DEVICE_REGISTRY_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend device preset data shared with callers of this public contract.
 */
typedef struct UmiFrontendDevicePreset { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char label[UMI_FRONTEND_DEV_NAME_CAPACITY]; UmiFrontendDeviceKind kind; UmiFrontendOrientation orientation; uint32_t width; uint32_t height; uint32_t scale_percent; int touch; } UmiFrontendDevicePreset;
/**
 * Represent the frontend device registry data shared with callers of this public contract.
 */
typedef struct UmiFrontendDeviceRegistry { UmiFrontendDevicePreset items[UMI_FRONTEND_DEV_MAX_DEVICES]; size_t count; uint64_t revision; } UmiFrontendDeviceRegistry;
/**
 * Initialise frontend device registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_device_registry_init(UmiFrontendDeviceRegistry *registry);
/**
 * Add frontend device registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_frontend_device_registry_add(UmiFrontendDeviceRegistry *registry,const UmiFrontendDevicePreset *preset);
/**
 * Find frontend device registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_device_registry_find(const UmiFrontendDeviceRegistry *registry,const char *id,UmiFrontendDevicePreset *out_preset);
/**
 * Provide the frontend device registry add defaults operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_device_registry_add_defaults(UmiFrontendDeviceRegistry *registry);
#endif
