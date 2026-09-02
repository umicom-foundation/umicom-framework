/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
 * File: tests/runtime_bootstrap/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for runtime bootstrap behaviour
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/types.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    char text[16];
    CHECK(umi_bootstrap_copy_text(text, sizeof(text), "alpha") == UMI_STATUS_OK);
    CHECK(strcmp(text, "alpha") == 0);
    CHECK(umi_bootstrap_id_valid("runtime.service-1"));
    CHECK(!umi_bootstrap_id_valid("runtime service"));
    CHECK(umi_bootstrap_hash_text("alpha") == umi_bootstrap_hash_text("alpha"));
    return 0;
}
