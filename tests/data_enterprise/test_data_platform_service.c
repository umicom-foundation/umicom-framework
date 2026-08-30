/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_platform_service.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data platform service enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_platform_service.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataPlatformService s; UmiDataServerProfile p; UmiDataHealth h; UmiDataOperation op; CHECK(umi_data_data_server_profile_init(&p,"prod",1U,8U,1000U)==UMI_STATUS_OK); CHECK(umi_data_data_platform_service_init(&s,"data.main",&p)==UMI_STATUS_OK); CHECK(umi_data_data_platform_service_start(&s)==UMI_STATUS_OK); CHECK(umi_data_health_compute(&h,UMI_DATA_ENTERPRISE_READY,UMI_DATA_ENTERPRISE_READY,UMI_DATA_ENTERPRISE_READY,false)==UMI_STATUS_OK); CHECK(umi_data_data_platform_service_update_health(&s,&h)==UMI_STATUS_OK); CHECK(umi_data_data_operation_init(&op,"op1","sess","query",1U,1U)==UMI_STATUS_OK); CHECK(umi_data_data_platform_service_submit(&s,&op)==UMI_STATUS_OK); CHECK(s.queue.count==1U);
    return 0;
}
