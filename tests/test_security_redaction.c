/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_redaction.c
 *
 * PURPOSE:
 *   Verify sensitive values are masked and ordinary values are preserved.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/security/redaction.h"
int main(void) {
    UmiRedactor *r = NULL; char out[64];
    assert(umi_redactor_create(&r) == UMI_STATUS_OK);
    assert(umi_redactor_copy_value(r, "api_token", "secret123", out, sizeof(out)) == UMI_STATUS_OK);
    assert(strcmp(out, "*********") == 0);
    assert(umi_redactor_copy_value(r, "username", "sammy", out, sizeof(out)) == UMI_STATUS_OK);
    assert(strcmp(out, "sammy") == 0);
    umi_redactor_destroy(r); return 0;
}
