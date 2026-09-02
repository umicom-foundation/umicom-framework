/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/dev_server.h
 *
 * PURPOSE:
 *   Publish the public dev server contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_DEV_SERVER_H
#define UMICOM_FRONTEND_DEV_SERVER_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend dev server config data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendDevServerConfig { char root[UMI_FRONTEND_DEV_PATH_CAPACITY]; char host[128]; uint16_t port; int single_page_fallback; int allow_remote; } UmiFrontendDevServerConfig;
/**
 * Represent the frontend dev server data shared with callers of this public contract.
 */
typedef struct UmiFrontendDevServer { UmiFrontendDevServerConfig config; char base_url[UMI_FRONTEND_DEV_URL_CAPACITY]; uint64_t request_count; uint64_t generation; int running; } UmiFrontendDevServer;
/**
 * Provide the frontend dev server config default operation used by this module and its
 * client applications.
 */
UmiFrontendDevServerConfig umi_frontend_dev_server_config_default(void);
/**
 * Initialise frontend dev server from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_dev_server_init(UmiFrontendDevServer *server,const UmiFrontendDevServerConfig *config);
/**
 * Provide the frontend dev server start operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dev_server_start(UmiFrontendDevServer *server);
/**
 * Provide the frontend dev server stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dev_server_stop(UmiFrontendDevServer *server);
/**
 * Provide the frontend dev server record request operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_dev_server_record_request(UmiFrontendDevServer *server);
#endif
