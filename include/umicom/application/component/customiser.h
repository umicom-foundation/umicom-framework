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

typedef struct UmiApplicationComponentCustomiser {
  UmiApplicationComponentWorkspaceDraft draft;
  UmiApplicationComponentHistory history;
  uint64_t saved_revision;
  size_t saved_cursor;
  int saved_cursor_valid;
} UmiApplicationComponentCustomiser;

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

UmiStatus umi_application_component_customiser_init(UmiApplicationComponentCustomiser *customiser,
                                                    const UmiApplicationComponentRecipe *recipe);
UmiStatus umi_application_component_customiser_add(UmiApplicationComponentCustomiser *customiser,
                                                   const char *component_id,
                                                   const char *instance_id,
                                                   UmiApplicationComponentRegion region,
                                                   uint32_t weight);
UmiStatus umi_application_component_customiser_remove(UmiApplicationComponentCustomiser *customiser,
                                                      const char *instance_id);
UmiStatus umi_application_component_customiser_move(UmiApplicationComponentCustomiser *customiser,
                                                    const char *instance_id, size_t new_index);
UmiStatus
umi_application_component_customiser_set_visible(UmiApplicationComponentCustomiser *customiser,
                                                 const char *instance_id, int visible);
UmiStatus
umi_application_component_customiser_set_region(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id,
                                                UmiApplicationComponentRegion region);
UmiStatus
umi_application_component_customiser_set_weight(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id, uint32_t weight);
UmiStatus
umi_application_component_customiser_set_title(UmiApplicationComponentCustomiser *customiser,
                                               const char *title);
UmiStatus umi_application_component_customiser_undo(UmiApplicationComponentCustomiser *customiser);
UmiStatus umi_application_component_customiser_redo(UmiApplicationComponentCustomiser *customiser);
void umi_application_component_customiser_mark_saved(UmiApplicationComponentCustomiser *customiser);
UmiStatus umi_application_component_customiser_snapshot(
    const UmiApplicationComponentCustomiser *customiser,
    UmiApplicationComponentCustomiserSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
