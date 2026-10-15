/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_constructor_binding.c
 *
 * PURPOSE:
 *   Implement the test constructor binding behavior for
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
 * File: tests/runtime_bootstrap/test_constructor_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Capture constructor-style dependency lists for C factory functions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/dependency_descriptor.h"
#include "umicom/runtime/bootstrap/constructor_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted constructor binding value. */
static int UmiBootstrapConstructorBindingConstructionCases(void)
{
    UmiBootstrapConstructorBinding *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_constructor_binding_init(value, "impl.alpha") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->implementation_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_constructor_binding_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_constructor_binding_init(value, value->implementation_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->implementation_id, "impl.alpha") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapConstructorBindingConstructionCases() != 0) return 1;


    UmiBootstrapConstructorBinding b; UmiBootstrapDependencyDescriptor d;
    CHECK(umi_bootstrap_constructor_binding_init(&b,"impl.alpha")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_dependency_descriptor_init(&d,"svc.data","",true,false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_constructor_binding_add_dependency(&b,&d)==UMI_STATUS_OK);
    CHECK(b.dependency_count==1U);
    return 0;
}
