/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_scan.c
 *
 * PURPOSE:
 *   Implement the test terminal scan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework portable/assembly terminal scanner tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/terminal/scan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char text[] = "alpha\nbeta\r\ngamma\n";
    UmiTerminalScanBackend backend = umi_terminal_scan_backend();
    assert(umi_terminal_scan_count_byte((const unsigned char *)text,
                                        strlen(text), (unsigned char)'a') == 5U);
    assert(umi_terminal_scan_line_count(text, strlen(text)) == 3U);
    assert(umi_terminal_scan_backend_name(backend) != NULL);
    assert(umi_terminal_scan_backend_name(UMI_TERMINAL_SCAN_PORTABLE_C) != NULL);
    return 0;
}
