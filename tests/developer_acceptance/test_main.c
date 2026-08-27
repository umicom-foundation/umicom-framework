/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused developer capability acceptance evidence, gate and toolchain-readiness tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_developer_acceptance_evidence(void);
int test_developer_acceptance_gate_pass(void);
int test_developer_acceptance_gate_missing(void);
int test_developer_acceptance_report(void);
int test_developer_acceptance_evidence_replace(void);
int test_developer_acceptance_gate_fail(void);
int test_developer_acceptance_gate_blocked(void);
int test_developer_acceptance_toolchain_gate_pass(void);
int test_developer_acceptance_toolchain_gate_blocked(void);
int test_developer_acceptance_report_empty(void);

int main(void)
{
    if (test_developer_acceptance_evidence() != 0) return 1;
    if (test_developer_acceptance_gate_pass() != 0) return 2;
    if (test_developer_acceptance_gate_missing() != 0) return 3;
    if (test_developer_acceptance_report() != 0) return 4;
    if (test_developer_acceptance_evidence_replace() != 0) return 5;
    if (test_developer_acceptance_gate_fail() != 0) return 6;
    if (test_developer_acceptance_gate_blocked() != 0) return 7;
    if (test_developer_acceptance_toolchain_gate_pass() != 0) return 8;
    if (test_developer_acceptance_toolchain_gate_blocked() != 0) return 9;
    if (test_developer_acceptance_report_empty() != 0) return 10;
    puts("developer acceptance tests passed");
    return 0;
}
