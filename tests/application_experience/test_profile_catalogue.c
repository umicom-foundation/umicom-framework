/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_profile_catalogue.c
 *
 * PURPOSE:
 *   Verify all 78 experience profiles are unique, valid and cover every
 *   application with Learning, Standard and Focus behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"
#include "umicom/application/portfolio.h"

#include <assert.h>

int main(void) {
  size_t index;
  assert(umi_application_experience_profile_catalogue_count() == 78U);
  assert(umi_application_experience_profile_catalogue_validate() == UMI_STATUS_OK);
  for (index = 0U; index < umi_application_portfolio_count(); ++index) {
    const UmiApplicationDefinition *application = umi_application_portfolio_at(index);
    assert(application != NULL);
    assert(umi_application_experience_profile_catalogue_application_count(
               application->application_id) == 3U);
    assert(umi_application_experience_profile_catalogue_recommend(
               application->application_id, UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD) !=
           NULL);
  }
  return 0;
}
