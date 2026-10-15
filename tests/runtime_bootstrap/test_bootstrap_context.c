/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_context.c
 *
 * PURPOSE:
 *   Implement the test bootstrap context behavior for
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
 * File: tests/runtime_bootstrap/test_bootstrap_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create the complete application bootstrap input owned by Framework runtime.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_context.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted bootstrap context value. */
static int UmiBootstrapContextConstructionCases(void)
{
    UmiBootstrapContext *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_context_init(value, "app.studio", "windows", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->application_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_context_init(value, oversized, "windows", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_context_init(value, value->application_id, "windows", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->application_id, "app.studio") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->platform_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_context_init(value, "app.studio", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_context_init(value, "app.studio", value->platform_id, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->platform_id, "windows") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapContextConstructionCases() != 0) return 1;


    UmiBootstrapContext c;
    CHECK(umi_bootstrap_context_init(&c,"app.studio","windows",true)==UMI_STATUS_OK);
    CHECK(c.dry_run && strcmp(c.application_id,"app.studio")==0);
    return 0;
}
