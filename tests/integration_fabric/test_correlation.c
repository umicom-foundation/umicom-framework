/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_correlation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the correlation Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/construction_cases.h"
#include "umicom/integration/fabric/correlation.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiFabricMessageHeaderTextEditCases(void)
{
    UmiFabricMessageHeader *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_correlation_set_causation(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->causation_id) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_correlation_set_causation(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_correlation_set_causation(value, value->causation_id + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->causation_id, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_correlation_set_causation(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricMessageHeaderTextEditCases() != 0) return 1;

    UmiFabricMessageHeader a,b; CHECK(umi_fabric_message_header_init(&a,"m1","c","t","x",1U)==UMI_STATUS_OK); CHECK(umi_fabric_message_header_init(&b,"m2","c","t","x",2U)==UMI_STATUS_OK); CHECK(umi_fabric_correlation_matches(&a,&b)); CHECK(umi_fabric_correlation_set_causation(&b,"m1")==UMI_STATUS_OK); CHECK(strcmp(b.causation_id,"m1")==0);
    return 0;
}
