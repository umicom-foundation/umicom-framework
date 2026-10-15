/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_os_service_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the os service catalogue cross-target capability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/os_service_catalogue.h"
#include "../value_archive/construction_cases.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted os service descriptor value. */
static int UmiCtOsServiceDescriptorConstructionCases(void)
{
    UmiCtOsServiceDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ct_os_service_descriptor_init(value, "kernel", UMI_CT_OS_SERVICE_KERNEL, true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->service_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ct_os_service_descriptor_init(value, oversized, UMI_CT_OS_SERVICE_KERNEL, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ct_os_service_descriptor_init(value, value->service_id, UMI_CT_OS_SERVICE_KERNEL, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->service_id, "kernel") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiCtOsServiceDescriptorConstructionCases() != 0) return 1;
UmiCtOsServiceCatalogue c;UmiCtOsServiceDescriptor s;umi_ct_os_service_catalogue_init(&c);CHECK(umi_ct_os_service_descriptor_init(&s,"kernel",UMI_CT_OS_SERVICE_KERNEL,true)==UMI_STATUS_OK);CHECK(umi_ct_os_service_catalogue_add(&c,&s)==UMI_STATUS_OK);CHECK(umi_ct_os_service_catalogue_find(&c,"kernel")!=NULL);return 0;}
