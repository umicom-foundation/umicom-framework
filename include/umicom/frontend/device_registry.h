/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_FRONTEND_DEVICE_REGISTRY_H
#define UMICOM_FRONTEND_DEVICE_REGISTRY_H
#include "umicom/frontend/dev_types.h"
typedef struct UmiFrontendDevicePreset { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char label[UMI_FRONTEND_DEV_NAME_CAPACITY]; UmiFrontendDeviceKind kind; UmiFrontendOrientation orientation; uint32_t width; uint32_t height; uint32_t scale_percent; int touch; } UmiFrontendDevicePreset;
typedef struct UmiFrontendDeviceRegistry { UmiFrontendDevicePreset items[UMI_FRONTEND_DEV_MAX_DEVICES]; size_t count; uint64_t revision; } UmiFrontendDeviceRegistry;
UmiStatus umi_frontend_device_registry_init(UmiFrontendDeviceRegistry *registry);
UmiStatus umi_frontend_device_registry_add(UmiFrontendDeviceRegistry *registry,const UmiFrontendDevicePreset *preset);
UmiStatus umi_frontend_device_registry_find(const UmiFrontendDeviceRegistry *registry,const char *id,UmiFrontendDevicePreset *out_preset);
UmiStatus umi_frontend_device_registry_add_defaults(UmiFrontendDeviceRegistry *registry);
#endif
