/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_link.c
 *
 * PURPOSE:
 *   Implement the test terminal link behavior for
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
 * Umicom Framework terminal link-detection and navigation tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/terminal_ui/navigation.h"

int main(void)
{
    UmiTerminalLinkResult result;
    UmiUiNavigationEntrySnapshot navigation;
    assert(umi_terminal_links_detect(
        "src/main.c:42:7 https://umicom.org support@umicom.org",
        &result) == UMI_STATUS_OK);
    assert(result.count == 3U);
    assert(result.links[0].kind == UMI_TERMINAL_LINK_COMPILER_LOCATION);
    assert(result.links[0].line == 42U && result.links[0].column == 7U);
    assert(strcmp(result.links[0].target, "src/main.c") == 0);
    assert(result.links[1].kind == UMI_TERMINAL_LINK_WEB);
    assert(result.links[2].kind == UMI_TERMINAL_LINK_EMAIL);
    assert(umi_terminal_ui_link_navigation(&result.links[0], &navigation) ==
           UMI_STATUS_OK);
    assert(navigation.line == 42U && navigation.column == 7U);
    return 0;
}
