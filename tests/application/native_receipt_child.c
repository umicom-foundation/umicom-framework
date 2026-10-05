/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/native_receipt_child.c
 * PURPOSE: Inert child that demonstrates exit status and executable-directory working context.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
int main(void)
{
    /* The fixture creates only its own marker, never launches a product or
     * reads user files. Delay lets the parent exercise an active duplicate. */
    g_usleep(150000U);
    if (!g_file_test("receipt-fixture", G_FILE_TEST_IS_REGULAR))
        return 19;
    int code = g_file_test("receipt-fail", G_FILE_TEST_IS_REGULAR) ? 7 : 0;
    return g_file_set_contents("receipt-finished", "finished", -1, NULL) ? code : 23;
}
