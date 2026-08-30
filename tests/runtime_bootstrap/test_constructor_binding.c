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
#include <string.h>
#include "umicom/runtime/bootstrap/dependency_descriptor.h"
#include "umicom/runtime/bootstrap/constructor_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapConstructorBinding b; UmiBootstrapDependencyDescriptor d;
    CHECK(umi_bootstrap_constructor_binding_init(&b,"impl.alpha")==UMI_STATUS_OK);
    CHECK(umi_bootstrap_dependency_descriptor_init(&d,"svc.data","",true,false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_constructor_binding_add_dependency(&b,&d)==UMI_STATUS_OK);
    CHECK(b.dependency_count==1U);
    return 0;
}
