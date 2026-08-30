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

int test_desktop_monitor_add_primary(void);
int test_desktop_monitor_duplicate_add(void);
int test_desktop_monitor_update_geometry(void);
int test_desktop_monitor_remove_primary(void);
int test_desktop_monitor_set_primary(void);
int test_desktop_monitor_find(void);
int test_desktop_monitor_constrain_negative(void);
int test_desktop_monitor_constrain_oversized(void);
int test_desktop_monitor_constrain_right_bottom(void);
int test_desktop_monitor_negative_origin(void);
int test_desktop_monitor_capacity(void);
int test_desktop_monitor_invalid_scale(void);
int test_desktop_monitor_invalid_refresh(void);
int test_desktop_monitor_remove_missing(void);
int test_desktop_monitor_constrain_disabled(void);

int main(void)
{
    if (test_desktop_monitor_add_primary() != 0) return 1;
    if (test_desktop_monitor_duplicate_add() != 0) return 2;
    if (test_desktop_monitor_update_geometry() != 0) return 3;
    if (test_desktop_monitor_remove_primary() != 0) return 4;
    if (test_desktop_monitor_set_primary() != 0) return 5;
    if (test_desktop_monitor_find() != 0) return 6;
    if (test_desktop_monitor_constrain_negative() != 0) return 7;
    if (test_desktop_monitor_constrain_oversized() != 0) return 8;
    if (test_desktop_monitor_constrain_right_bottom() != 0) return 9;
    if (test_desktop_monitor_negative_origin() != 0) return 10;
    if (test_desktop_monitor_capacity() != 0) return 11;
    if (test_desktop_monitor_invalid_scale() != 0) return 12;
    if (test_desktop_monitor_invalid_refresh() != 0) return 13;
    if (test_desktop_monitor_remove_missing() != 0) return 14;
    if (test_desktop_monitor_constrain_disabled() != 0) return 15;
    puts("desktop monitor acceptance tests passed");
    return 0;
}
