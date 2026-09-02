/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_builtin_catalogues.c
 *
 * PURPOSE:
 *   Verify source-control operations, terminal profiles and diagnostic parsers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/builtin_diagnostics.h"
#include "umicom/developer_productivity/builtin_source_control_operations.h"
#include "umicom/developer_productivity/builtin_terminal_profiles.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_developer_builtin_source_control_operation_count() ==
           13U);
    assert(umi_developer_builtin_terminal_profile_count() ==
           6U);
    assert(umi_developer_builtin_diagnostic_parser_count() ==
           12U);
    return 0;
}
