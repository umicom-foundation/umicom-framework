/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_find_missing.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix find missing behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/developer/language_support_matrix.h"
int test_language_support_find_missing(void)
{
    UmiDeveloperLanguageSupportMatrix matrix={0};
    assert(umi_developer_language_support_matrix_find(
        &matrix,"missing")==NULL);
    return 0;
}
