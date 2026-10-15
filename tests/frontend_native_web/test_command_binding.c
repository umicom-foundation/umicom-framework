/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_command_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web command binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/frontend/native_web/command_binding.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted command binding value. */
static int UmiNativeWebCommandBindingConstructionCases(void)
{
    UmiNativeWebCommandBinding *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_native_web_command_binding_init(value, "btn", "build", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->element_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_command_binding_init(value, oversized, "build", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_command_binding_init(value, value->element_id, "build", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->element_id, "btn") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->command_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_command_binding_init(value, "btn", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_native_web_command_binding_init(value, "btn", value->command_id, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->command_id, "build") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiNativeWebCommandBindingConstructionCases() != 0) return 1;

    UmiNativeWebCommandBinding b; CHECK(umi_native_web_command_binding_init(&b,"btn","build",true)==UMI_STATUS_OK); CHECK(b.enabled);
    return 0;
}
