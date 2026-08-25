/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_context.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create the complete application bootstrap input owned by Framework runtime.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_context.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapContext c;
    CHECK(umi_bootstrap_context_init(&c,"app.studio","windows",true)==UMI_STATUS_OK);
    CHECK(c.dry_run && strcmp(c.application_id,"app.studio")==0);
    return 0;
}
