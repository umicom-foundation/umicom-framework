/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_monitor_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused acceptance checks over the existing Framework desktop monitor-topology service.
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
 * Exercise test desktop monitor add primary and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_desktop_monitor_add_primary(void);
/*
 * Add test desktop monitor duplicate only after its inputs and available capacity have
 * been checked.
 */
int test_desktop_monitor_duplicate_add(void);
/*
 * Exercise test desktop monitor update geometry and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_update_geometry(void);
/*
 * Exercise test desktop monitor remove primary and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_remove_primary(void);
/*
 * Exercise test desktop monitor set primary and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_desktop_monitor_set_primary(void);
/*
 * Find test desktop monitor while leaving the underlying catalogue or model owned by this
 * module.
 */
int test_desktop_monitor_find(void);
/*
 * Exercise test desktop monitor constrain negative and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_negative(void);
/*
 * Exercise test desktop monitor constrain oversized and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_oversized(void);
/*
 * Exercise test desktop monitor constrain right bottom and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_right_bottom(void);
/*
 * Exercise test desktop monitor negative origin and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_negative_origin(void);
/*
 * Exercise test desktop monitor capacity and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_desktop_monitor_capacity(void);
/*
 * Exercise test desktop monitor invalid scale and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_desktop_monitor_invalid_scale(void);
/*
 * Exercise test desktop monitor invalid refresh and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_invalid_refresh(void);
/*
 * Exercise test desktop monitor remove missing and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_remove_missing(void);
/*
 * Exercise test desktop monitor constrain disabled and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_desktop_monitor_constrain_disabled(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_add_primary() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_duplicate_add() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_update_geometry() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_remove_primary() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_set_primary() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_find() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_constrain_negative() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_constrain_oversized() != 0) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_constrain_right_bottom() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_negative_origin() != 0) return 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_capacity() != 0) return 11;
    /* Apply this operation only while the related capability or state is available. */
    if (test_desktop_monitor_invalid_scale() != 0) return 12;
    /* Apply this operation only while the related capability or state is available. */
    if (test_desktop_monitor_invalid_refresh() != 0) return 13;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_remove_missing() != 0) return 14;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_desktop_monitor_constrain_disabled() != 0) return 15;
    puts("desktop monitor acceptance tests passed");
    return 0;
}
