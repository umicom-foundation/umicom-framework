/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/preset_set.h
 *
 * PURPOSE:
 *   Review and select matching configure, build and test preset names without executing tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_PRESET_SET_H
#define UMICOM_DEVELOPER_PROJECT_PRESET_SET_H
#include "umicom/developer_project/preset_catalogue.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProjectPresetSetSummary {
  size_t configure_index, build_count, test_count;
  /* An index is SIZE_MAX unless exactly one eligible candidate exists. */
  size_t build_index, test_index;
  bool discovery_incomplete, conditions_deferred;
} UmiProjectPresetSetSummary;
/** Inspect build/test presets whose declared or resolved configurePreset exactly
 * matches the selected configure preset's name. Hidden and literal-false choices
 * are excluded. Conditions needing evaluation remain marked deferred.
 * A non-configure index is INVALID_ARGUMENT; hidden/false configure choices are
 * PERMISSION_DENIED. Other failures preserve output.
 *
 * Direct discovery with unread includes or unresolved inheritance is incomplete.
 * Use the existing explicit expanded read to resolve supported local files.
 * These functions do not read files, evaluate conditions, alter a catalogue,
 * grant trust, save settings or run a tool. Calls borrow the catalogue. */
UmiStatus UmiProjectPresetSetInspect(const UmiProjectPresetCatalogue *catalogue,
                                     size_t configureIndex, UmiProjectPresetSetSummary *out);
/** Copy all three stage names only when discovery is complete and each related
 * stage has exactly one eligible candidate. Missing stages return NOT_FOUND;
 * multiple candidates return ALREADY_EXISTS; unresolved discovery returns
 * NOT_IMPLEMENTED. Choose individual names when a project needs another policy.
 * A legacy shared preset must be explicitly cleared first. All other profile
 * fields remain unchanged. Failure preserves output; in-place selection is valid.
 * CMake still validates files and conditions when the owner starts each stage. */
UmiStatus UmiProjectPresetSetSelect(const UmiProjectPresetCatalogue *catalogue,
                                    size_t configureIndex, const UmiBuildProfile *profile,
                                    UmiBuildProfile *out);
#ifdef __cplusplus
}
#endif
#endif
