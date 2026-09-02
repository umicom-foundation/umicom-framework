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
/**
 * Represent the frontend preview session data shared with callers of this public contract.
 */
typedef struct UmiFrontendPreviewSession { char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char url[UMI_FRONTEND_DEV_URL_CAPACITY]; char active_document[UMI_FRONTEND_DEV_PATH_CAPACITY]; UmiFrontendDevicePreset device; uint64_t revision; uint64_t reload_count; int connected; int inspect_mode; } UmiFrontendPreviewSession;
/**
 * Initialise frontend preview session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_preview_session_init(UmiFrontendPreviewSession *session,const char *id,const char *url,const UmiFrontendDevicePreset *device);
/**
 * Provide the frontend preview session navigate operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_preview_session_navigate(UmiFrontendPreviewSession *session,const char *url);
/**
 * Provide the frontend preview session reload operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_preview_session_reload(UmiFrontendPreviewSession *session);
/**
 * Provide the frontend preview session set device operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_preview_session_set_device(UmiFrontendPreviewSession *session,const UmiFrontendDevicePreset *device);
#endif
