/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused developer capability acceptance evidence, gate and toolchain-readiness tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

/*
 * Exercise test developer acceptance evidence and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_developer_acceptance_evidence(void);
/*
 * Exercise test developer acceptance gate pass and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_gate_pass(void);
/*
 * Exercise test developer acceptance gate missing and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_gate_missing(void);
/*
 * Exercise test developer acceptance report and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_developer_acceptance_report(void);
/*
 * Exercise test developer acceptance evidence replace and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_evidence_replace(void);
/*
 * Exercise test developer acceptance gate fail and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_gate_fail(void);
/*
 * Exercise test developer acceptance gate blocked and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_gate_blocked(void);
/*
 * Exercise test developer acceptance toolchain gate pass and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_developer_acceptance_toolchain_gate_pass(void);
/*
 * Exercise test developer acceptance toolchain gate blocked and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_developer_acceptance_toolchain_gate_blocked(void);
/*
 * Exercise test developer acceptance report empty and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_report_empty(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_evidence() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_gate_pass() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_gate_missing() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_report() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_evidence_replace() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_gate_fail() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_gate_blocked() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_toolchain_gate_pass() != 0) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_toolchain_gate_blocked() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_developer_acceptance_report_empty() != 0) return 10;
    puts("developer acceptance tests passed");
    return 0;
}
