/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_dev_server_preview.c
 *
 * PURPOSE:
 *   Implement the test frontend dev server preview behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/frontend/frontend_studio.h"
int main(void){UmiFrontendDevServerConfig c=umi_frontend_dev_server_config_default();UmiFrontendDevServer s;UmiFrontendDeviceRegistry r;UmiFrontendDevicePreset d;UmiFrontendPreviewSession p;assert(umi_frontend_dev_server_init(&s,&c)==UMI_STATUS_OK);assert(umi_frontend_dev_server_start(&s)==UMI_STATUS_OK);assert(umi_frontend_dev_server_record_request(&s)==UMI_STATUS_OK&&s.request_count==1U);assert(umi_frontend_device_registry_init(&r)==UMI_STATUS_OK);assert(umi_frontend_device_registry_add_defaults(&r)==UMI_STATUS_OK);assert(umi_frontend_device_registry_find(&r,"desktop-1440",&d)==UMI_STATUS_OK);assert(umi_frontend_preview_session_init(&p,"primary",s.base_url,&d)==UMI_STATUS_OK);assert(strstr(p.url,"4310")!=NULL);assert(umi_frontend_preview_session_reload(&p)==UMI_STATUS_OK&&p.reload_count==1U);return 0;}
