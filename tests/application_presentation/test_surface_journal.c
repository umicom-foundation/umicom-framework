/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_journal.c
 *
 * PURPOSE:
 *   Verify the presentation event journal stays bounded and retains the newest
 *   diagnostic outcomes in sequence order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/presentation/presentation.h"

int main(void)
{
    UmiApplicationPresentationSurfaceJournal journal;
    size_t index;
    umi_application_presentation_surface_journal_init(&journal);
    for (index = 0U; index < UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY + 5U;
         ++index) {
        umi_application_presentation_surface_journal_record(
            &journal, UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
            "umicom.shared.diagnostics", UMI_STATUS_OK);
    }
    assert(journal.count == UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY);
    assert(umi_application_presentation_surface_journal_at(&journal, 0U)->sequence == 6U);
    assert(umi_application_presentation_surface_journal_at(
               &journal, journal.count - 1U)->sequence ==
           UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY + 5U);
    return 0;
}
