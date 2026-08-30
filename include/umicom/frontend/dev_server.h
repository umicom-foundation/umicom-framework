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
typedef struct UmiFrontendDevServerConfig { char root[UMI_FRONTEND_DEV_PATH_CAPACITY]; char host[128]; uint16_t port; int single_page_fallback; int allow_remote; } UmiFrontendDevServerConfig;
typedef struct UmiFrontendDevServer { UmiFrontendDevServerConfig config; char base_url[UMI_FRONTEND_DEV_URL_CAPACITY]; uint64_t request_count; uint64_t generation; int running; } UmiFrontendDevServer;
UmiFrontendDevServerConfig umi_frontend_dev_server_config_default(void);
UmiStatus umi_frontend_dev_server_init(UmiFrontendDevServer *server,const UmiFrontendDevServerConfig *config);
UmiStatus umi_frontend_dev_server_start(UmiFrontendDevServer *server);
UmiStatus umi_frontend_dev_server_stop(UmiFrontendDevServer *server);
UmiStatus umi_frontend_dev_server_record_request(UmiFrontendDevServer *server);
#endif
