/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_text_scan.c
 *
 * PURPOSE:
 *   Implement the test editor text scan behavior for
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
 * Umicom Framework C/assembly scan equivalence tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/editor/text_scan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char TEXT[] = "one two\r\nthree\tfour\n\xC2\xA3";
    UmiEditorTextScanProfile profile;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_text_scan_profile(TEXT, sizeof(TEXT) - 1U, &profile) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile.byte_count != sizeof(TEXT) - 1U ||
        profile.line_feed_count != 2U ||
        profile.carriage_return_count != 1U ||
        profile.tab_count != 1U ||
        profile.space_count != 1U ||
        profile.non_ascii_count != 2U) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_text_scan_count_byte((const unsigned char *)TEXT,
            sizeof(TEXT) - 1U, (unsigned char)'o') != 3U) return 3;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_editor_text_scan_backend_name(profile.backend) == NULL) return 4;
    return EXIT_SUCCESS;
}
