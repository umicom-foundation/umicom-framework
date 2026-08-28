/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_component_governance_evidence.c
 *
 * PURPOSE:
 *   Verify known evidence and frontend flags, missing-evidence calculations and
 *   safe completion of an existing candidate component record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void) {
  UmiComponentGovernanceRecord record;
  const UmiApplicationComponentDefinition *definition =
      umi_application_component_catalogue_find("umicom.shell.command-palette");
  uint32_t missing;

  assert(definition != NULL);
  assert(umi_component_governance_default(definition, &record) == UMI_STATUS_OK);
  assert(record.api_status == UMI_COMPONENT_API_CANDIDATE);
  assert(!umi_component_evidence_is_complete(&record));
  missing = umi_component_evidence_missing(&record);
  assert((missing & UMI_COMPONENT_EVIDENCE_UNIT_TEST) != 0U);
  assert((missing & UMI_COMPONENT_EVIDENCE_ACCESSIBILITY) != 0U);
  assert(umi_component_evidence_add(&record, missing) == UMI_STATUS_OK);
  assert(umi_component_frontend_add(&record, UMI_COMPONENT_FRONTEND_HEADLESS |
                                                 UMI_COMPONENT_FRONTEND_GTK4) == UMI_STATUS_OK);
  assert(umi_component_evidence_is_complete(&record));
  assert(umi_component_flag_count(record.available_evidence) == 8U);
  assert(umi_component_evidence_add(&record, 1U << 30) == UMI_STATUS_INVALID_ARGUMENT);
  assert(umi_component_frontend_add(&record, 1U << 30) == UMI_STATUS_INVALID_ARGUMENT);
  assert(umi_component_evidence_text(UMI_COMPONENT_EVIDENCE_HEADLESS)[0] != '\0');
  assert(umi_component_frontend_text(UMI_COMPONENT_FRONTEND_GTK4)[0] != '\0');
  return 0;
}
