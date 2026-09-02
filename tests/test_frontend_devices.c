/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_frontend_devices.c
 *
 * PURPOSE:
 *   Implement the test frontend devices behavior for
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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiFrontendDeviceRegistry r;UmiFrontendDevicePreset p;assert(umi_frontend_device_registry_init(&r)==UMI_STATUS_OK);assert(umi_frontend_device_registry_add_defaults(&r)==UMI_STATUS_OK);assert(r.count==3U);assert(umi_frontend_device_registry_find(&r,"phone-390",&p)==UMI_STATUS_OK);assert(p.touch&&p.width==390U&&strcmp(umi_frontend_device_kind_text(p.kind),"phone")==0);return 0;}
