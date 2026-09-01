/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/activity.h
 *
 * PURPOSE:
 *   Define the toolkit-neutral Activity Bar model used to switch major workbench areas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An activity is a stable navigation choice such as Explorer, Search, Source Control or Designer. GTK4, Qt, Wt and headless adapters render the same records.
 */

#ifndef UMICOM_UI_ACTIVITY_H
#define UMICOM_UI_ACTIVITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_ACTIVITY_MAX 64U

typedef struct UmiUiActivitySnapshot {
    char activity_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    char container_id[UMI_UI_ID_CAPACITY];
    int32_t order;
    uint32_t badge_count;
    int visible;
    int enabled;
    int active;
} UmiUiActivitySnapshot;

typedef struct UmiUiActivityModel UmiUiActivityModel;

UmiStatus umi_ui_activity_model_create(UmiUiActivityModel **out_model);
void umi_ui_activity_model_destroy(UmiUiActivityModel *model);
UmiStatus umi_ui_activity_model_upsert(UmiUiActivityModel *model,
                                       const UmiUiActivitySnapshot *item);
UmiStatus umi_ui_activity_model_remove(UmiUiActivityModel *model,
                                       const char *activity_id);
UmiStatus umi_ui_activity_model_find(const UmiUiActivityModel *model,
                                     const char *activity_id,
                                     UmiUiActivitySnapshot *out_item);
UmiStatus umi_ui_activity_model_at(const UmiUiActivityModel *model,
                                   size_t index,
                                   UmiUiActivitySnapshot *out_item);
UmiStatus umi_ui_activity_model_set_active(UmiUiActivityModel *model,
                                          const char *activity_id);
size_t umi_ui_activity_model_count(const UmiUiActivityModel *model);
uint64_t umi_ui_activity_model_revision(const UmiUiActivityModel *model);

#ifdef __cplusplus
}
#endif
#endif
