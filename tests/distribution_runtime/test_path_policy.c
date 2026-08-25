/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_path_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for portable path validation rejecting traversal and unsafe absolute package paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/path_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    CHECK(umi_dr_path_policy_relative_safe("bin/app.exe")); CHECK(!umi_dr_path_policy_relative_safe("../secret")); CHECK(!umi_dr_path_policy_relative_safe("C:\\app.exe"));
    return 0;
}
