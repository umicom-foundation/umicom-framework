/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_catalogue.c
 *
 * PURPOSE:
 *   Implement the test starter catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Maintain Framework-owned application starter definitions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/starter_descriptor.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted starter descriptor value. */
static int UmiBootstrapStarterDescriptorConstructionCases(void)
{
    UmiBootstrapStarterDescriptor *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_starter_descriptor_init(value, "starter.web", "feature.web", 5) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->starter_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_starter_descriptor_init(value, oversized, "feature.web", 5) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_starter_descriptor_init(value, value->starter_id, "feature.web", 5) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->starter_id, "starter.web") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->feature_pack_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_starter_descriptor_init(value, "starter.web", oversized, 5) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_starter_descriptor_init(value, "starter.web", value->feature_pack_id, 5) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->feature_pack_id, "feature.web") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapStarterDescriptorConstructionCases() != 0) return 1;


    UmiBootstrapStarterDescriptor s; UmiBootstrapStarterCatalogue c;
    umi_bootstrap_starter_catalogue_init(&c);
    CHECK(umi_bootstrap_starter_descriptor_init(&s,"starter.web","feature.web",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_add(&c,&s)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_find(&c,"starter.web")!=NULL);
    return 0;
}
