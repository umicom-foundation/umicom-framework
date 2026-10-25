/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/preset_set.c
 *
 * PURPOSE:
 *   Match declared build stages to one configure preset and publish a complete profile candidate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/preset_set.h"
#include <string.h>
static bool PresetSetUnresolved(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                const UmiProjectPresetChoice *choice) {
  if (!choice->inherits)
    return false;
  UmiProjectPresetOrigin origin;
  return UmiProjectPresetCatalogueOriginAt(catalogue, index, &origin) != UMI_STATUS_OK;
}
UmiStatus UmiProjectPresetSetInspect(const UmiProjectPresetCatalogue *catalogue,
                                     size_t configureIndex, UmiProjectPresetSetSummary *out) {
  if (catalogue == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiProjectPresetSummary snapshot;
  UmiProjectPresetChoice configure;
  UmiStatus status = UmiProjectPresetCatalogueSummary(catalogue, &snapshot);
  if (status == UMI_STATUS_OK)
    status = UmiProjectPresetCatalogueAt(catalogue, configureIndex, &configure);
  if (status != UMI_STATUS_OK)
    return status;
  if (configure.stage != UMI_PROJECT_PRESET_CONFIGURE)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (configure.hidden || configure.condition_false)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiProjectPresetSetSummary result = {0};
  result.configure_index = configureIndex;
  result.build_index = result.test_index = SIZE_MAX;
  result.discovery_incomplete =
      snapshot.includes_not_read || PresetSetUnresolved(catalogue, configureIndex, &configure);
  result.conditions_deferred = configure.condition_deferred;
  for (size_t i = 0U; i < snapshot.count; ++i) {
    UmiProjectPresetChoice candidate;
    status = UmiProjectPresetCatalogueAt(catalogue, i, &candidate);
    if (status != UMI_STATUS_OK)
      return status;
    if (candidate.stage == UMI_PROJECT_PRESET_CONFIGURE || candidate.hidden ||
        candidate.condition_false)
      continue;
    /* An unresolved parent can supply a configure reference or condition.
     * Do not interpret its missing field as proof that it is unrelated. */
    if (PresetSetUnresolved(catalogue, i, &candidate))
      result.discovery_incomplete = true;
    if (strcmp(candidate.configure_preset, configure.name) != 0)
      continue;
    result.conditions_deferred = result.conditions_deferred || candidate.condition_deferred;
    if (candidate.stage == UMI_PROJECT_PRESET_BUILD) {
      ++result.build_count;
      result.build_index = i;
    } else if (candidate.stage == UMI_PROJECT_PRESET_TEST) {
      ++result.test_count;
      result.test_index = i;
    }
  }
  if (result.build_count != 1U)
    result.build_index = SIZE_MAX;
  if (result.test_count != 1U)
    result.test_index = SIZE_MAX;
  *out = result;
  return UMI_STATUS_OK;
}
UmiStatus UmiProjectPresetSetSelect(const UmiProjectPresetCatalogue *catalogue,
                                    size_t configureIndex, const UmiBuildProfile *profile,
                                    UmiBuildProfile *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
  if (status != UMI_STATUS_OK)
    return status;
  if (profile->preset[0] != '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiProjectPresetSetSummary summary;
  status = UmiProjectPresetSetInspect(catalogue, configureIndex, &summary);
  if (status != UMI_STATUS_OK)
    return status;
  if (summary.discovery_incomplete)
    return UMI_STATUS_NOT_IMPLEMENTED;
  if (summary.build_count == 0U || summary.test_count == 0U)
    return UMI_STATUS_NOT_FOUND;
  if (summary.build_count != 1U || summary.test_count != 1U)
    return UMI_STATUS_ALREADY_EXISTS;
  /* Keep the output untouched until all three checked selections succeeded.
   * No preset name is inferred from another stage's spelling. */
  UmiBuildProfile selected = *profile;
  status = UmiProjectPresetCatalogueSelect(catalogue, configureIndex, &selected, &selected);
  if (status == UMI_STATUS_OK)
    status = UmiProjectPresetCatalogueSelect(catalogue, summary.build_index, &selected, &selected);
  if (status == UMI_STATUS_OK)
    status = UmiProjectPresetCatalogueSelect(catalogue, summary.test_index, &selected, &selected);
  if (status == UMI_STATUS_OK)
    *out = selected;
  return status;
}
