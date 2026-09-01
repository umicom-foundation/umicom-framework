/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/dev_types.h
 *
 * PURPOSE:
 *   Publish the public dev types contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* These bounded values cross toolkit and process boundaries
 * without exposing GTK, Qt, Wt, WebKit or browser-engine objects. */
#ifndef UMICOM_FRONTEND_DEV_TYPES_H
#define UMICOM_FRONTEND_DEV_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FRONTEND_DEV_ID_CAPACITY 128U
#define UMI_FRONTEND_DEV_NAME_CAPACITY 256U
#define UMI_FRONTEND_DEV_PATH_CAPACITY 1024U
#define UMI_FRONTEND_DEV_URL_CAPACITY 2048U
#define UMI_FRONTEND_DEV_TEXT_CAPACITY 4096U
#define UMI_FRONTEND_DEV_CONTENT_CAPACITY 65536U
#define UMI_FRONTEND_DEV_MAX_DEVICES 32U
#define UMI_FRONTEND_DEV_MAX_DOCUMENTS 128U
#define UMI_FRONTEND_DEV_MAX_DOM_NODES 512U
#define UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES 256U
#define UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES 256U
#define UMI_FRONTEND_DEV_MAX_BUILD_PROFILES 32U
#define UMI_FRONTEND_DEV_MAX_CHANGES 128U
typedef enum UmiFrontendDeviceKind { UMI_FRONTEND_DEVICE_DESKTOP=1, UMI_FRONTEND_DEVICE_TABLET=2, UMI_FRONTEND_DEVICE_PHONE=3, UMI_FRONTEND_DEVICE_CUSTOM=4 } UmiFrontendDeviceKind;
typedef enum UmiFrontendOrientation { UMI_FRONTEND_ORIENTATION_PORTRAIT=1, UMI_FRONTEND_ORIENTATION_LANDSCAPE=2 } UmiFrontendOrientation;
typedef enum UmiFrontendConsoleLevel { UMI_FRONTEND_CONSOLE_DEBUG=1, UMI_FRONTEND_CONSOLE_INFO=2, UMI_FRONTEND_CONSOLE_WARNING=3, UMI_FRONTEND_CONSOLE_ERROR=4 } UmiFrontendConsoleLevel;
typedef enum UmiFrontendResourceKind { UMI_FRONTEND_RESOURCE_DOCUMENT=1, UMI_FRONTEND_RESOURCE_STYLESHEET=2, UMI_FRONTEND_RESOURCE_SCRIPT=3, UMI_FRONTEND_RESOURCE_IMAGE=4, UMI_FRONTEND_RESOURCE_FONT=5, UMI_FRONTEND_RESOURCE_API=6, UMI_FRONTEND_RESOURCE_OTHER=7 } UmiFrontendResourceKind;
UmiStatus umi_frontend_dev_copy_text(char *destination,size_t capacity,const char *source);
const char *umi_frontend_device_kind_text(UmiFrontendDeviceKind kind);
const char *umi_frontend_console_level_text(UmiFrontendConsoleLevel level);
#ifdef __cplusplus
}
#endif
#endif
