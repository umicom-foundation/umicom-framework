/*-----------------------------------------------------------------------------
 * Umicom Framework ANSI SGR projection tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/terminal/ansi.h"

int main(void)
{
    UmiTerminalAnsiDocument document;
    assert(umi_terminal_ansi_parse(
        "plain \x1b[1;31merror\x1b[0m done", &document) == UMI_STATUS_OK);
    assert(strcmp(document.text, "plain error done") == 0);
    assert(document.segment_count == 3U);
    assert(document.segments[1].style.bold);
    assert(document.segments[1].style.foreground !=
           UMI_TERMINAL_ANSI_DEFAULT_COLOUR);
    assert(document.segments[2].style.foreground ==
           UMI_TERMINAL_ANSI_DEFAULT_COLOUR);
    return 0;
}
