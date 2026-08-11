/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_release_notes_platforms.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/release_note.h"
#include "umicom/delivery/platform_matrix.h"
int main(void) {
    UmiReleaseNotes notes;
    UmiPlatformMatrix matrix;
    umi_release_notes_init(&notes);
    umi_platform_matrix_init(&matrix);
    assert(umi_release_notes_add(&notes, "feature", "delivery platform") == UMI_STATUS_OK);
    assert(umi_platform_matrix_add(&matrix, "windows", "x86_64") == UMI_STATUS_OK);
    assert(umi_platform_matrix_supports(&matrix, "windows", "x86_64"));
    return 0;
}
