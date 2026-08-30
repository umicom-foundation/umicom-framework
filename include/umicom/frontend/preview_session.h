/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/preview_session.h
 *
 * PURPOSE:
 *   Publish the public preview session contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_PREVIEW_SESSION_H
#define UMICOM_FRONTEND_PREVIEW_SESSION_H
#include "umicom/frontend/device_registry.h"
typedef struct UmiFrontendPreviewSession { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char url[UMI_FRONTEND_DEV_URL_CAPACITY]; char active_document[UMI_FRONTEND_DEV_PATH_CAPACITY]; UmiFrontendDevicePreset device; uint64_t revision; uint64_t reload_count; int connected; int inspect_mode; } UmiFrontendPreviewSession;
UmiStatus umi_frontend_preview_session_init(UmiFrontendPreviewSession *session,const char *id,const char *url,const UmiFrontendDevicePreset *device);
UmiStatus umi_frontend_preview_session_navigate(UmiFrontendPreviewSession *session,const char *url);
UmiStatus umi_frontend_preview_session_reload(UmiFrontendPreviewSession *session);
UmiStatus umi_frontend_preview_session_set_device(UmiFrontendPreviewSession *session,const UmiFrontendDevicePreset *device);
#endif
