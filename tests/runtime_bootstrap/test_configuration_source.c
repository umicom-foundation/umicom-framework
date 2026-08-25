/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_source.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe trusted configuration sources and their precedence.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_source.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapConfigurationSource s;
    CHECK(umi_bootstrap_configuration_source_init(&s,"env",20,true)==UMI_STATUS_OK);
    CHECK(s.trusted && s.precedence==20);
    return 0;
}
