/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/welcome_view.h
 *
 * PURPOSE:
 *   Define contextual welcome content for empty or not-yet-configured views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_WELCOME_VIEW_H
#define UMICOM_UI_WELCOME_VIEW_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WELCOME_VIEW_CAPACITY 512U

typedef struct UmiUiWelcomeItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char view_id[128];
    char title[256];
    char description[512];
    char command_id[128];
    char when_expression[256];
    int32_t order;
    uint64_t revision;
} UmiUiWelcomeItemSnapshot;

typedef struct UmiUiWelcomeItemRegistry UmiUiWelcomeItemRegistry;

UmiStatus umi_ui_welcome_view_registry_create(UmiUiWelcomeItemRegistry **out_registry);
void umi_ui_welcome_view_registry_destroy(UmiUiWelcomeItemRegistry *registry);
UmiStatus umi_ui_welcome_view_registry_upsert(UmiUiWelcomeItemRegistry *registry, const UmiUiWelcomeItemSnapshot *item);
UmiStatus umi_ui_welcome_view_registry_remove(UmiUiWelcomeItemRegistry *registry, const char *id);
UmiStatus umi_ui_welcome_view_registry_find(const UmiUiWelcomeItemRegistry *registry, const char *id, UmiUiWelcomeItemSnapshot *out_item);
UmiStatus umi_ui_welcome_view_registry_at(const UmiUiWelcomeItemRegistry *registry, size_t index, UmiUiWelcomeItemSnapshot *out_item);
size_t umi_ui_welcome_view_registry_count(const UmiUiWelcomeItemRegistry *registry);
uint64_t umi_ui_welcome_view_registry_revision(const UmiUiWelcomeItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
