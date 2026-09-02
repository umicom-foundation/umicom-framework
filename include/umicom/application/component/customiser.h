/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/customiser.h
 *
 * PURPOSE:
 *   Provide the high-level layout editing service used by visual designers,
 *   command-line tools and future application settings screens.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_CUSTOMISER_H
#define UMICOM_APPLICATION_COMPONENT_CUSTOMISER_H

#include "umicom/application/component/history.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application component customiser data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentCustomiser {
  UmiApplicationComponentWorkspaceDraft draft;
  UmiApplicationComponentHistory history;
  uint64_t saved_revision;
  size_t saved_cursor;
  int saved_cursor_valid;
} UmiApplicationComponentCustomiser;

/**
 * Represent the application component customiser snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentCustomiserSnapshot {
  const char *recipe_id;
  const char *application_id;
  const char *title;
  size_t slot_count;
  size_t visible_count;
  size_t undo_count;
  size_t redo_count;
  uint64_t revision;
  int dirty;
} UmiApplicationComponentCustomiserSnapshot;

/**
 * Initialise application component customiser from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_component_customiser_init(UmiApplicationComponentCustomiser *customiser,
                                                    const UmiApplicationComponentRecipe *recipe);
/**
 * Add application component customiser only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_component_customiser_add(UmiApplicationComponentCustomiser *customiser,
                                                   const char *component_id,
                                                   const char *instance_id,
                                                   UmiApplicationComponentRegion region,
                                                   uint32_t weight);
/**
 * Remove application component customiser while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_application_component_customiser_remove(UmiApplicationComponentCustomiser *customiser,
                                                      const char *instance_id);
/**
 * Provide the application component customiser move operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_customiser_move(UmiApplicationComponentCustomiser *customiser,
                                                    const char *instance_id, size_t new_index);
/**
 * Provide the application component customiser set visible operation used by this module
 * and its client applications.
 */
UmiStatus
umi_application_component_customiser_set_visible(UmiApplicationComponentCustomiser *customiser,
                                                 const char *instance_id, int visible);
/**
 * Provide the application component customiser set region operation used by this module
 * and its client applications.
 */
UmiStatus
umi_application_component_customiser_set_region(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id,
                                                UmiApplicationComponentRegion region);
/**
 * Provide the application component customiser set weight operation used by this module
 * and its client applications.
 */
UmiStatus
umi_application_component_customiser_set_weight(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id, uint32_t weight);
/**
 * Provide the application component customiser set title operation used by this module and
 * its client applications.
 */
UmiStatus
umi_application_component_customiser_set_title(UmiApplicationComponentCustomiser *customiser,
                                               const char *title);
/**
 * Provide the application component customiser undo operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_customiser_undo(UmiApplicationComponentCustomiser *customiser);
/**
 * Provide the application component customiser redo operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_customiser_redo(UmiApplicationComponentCustomiser *customiser);
/**
 * Provide the application component customiser mark saved operation used by this module
 * and its client applications.
 */
void umi_application_component_customiser_mark_saved(UmiApplicationComponentCustomiser *customiser);
/**
 * Provide the application component customiser snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_customiser_snapshot(
    const UmiApplicationComponentCustomiser *customiser,
    UmiApplicationComponentCustomiserSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
