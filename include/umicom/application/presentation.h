/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation.h
 *
 * PURPOSE:
 *   Describe how independently versioned Umicom applications appear in the
 *   Framework-owned Umicom Desk, taskbar, launcher and layout catalogue.
 *
 * DESIGN NOTE:
 *   This metadata does not make an application executable. Installation,
 *   compatibility, permissions and runtime health remain independently checked
 *   before the Desk exposes a launch or switch action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_H
#define UMICOM_APPLICATION_PRESENTATION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiApplicationEntryKind {
    UMI_APPLICATION_ENTRY_WORKBENCH = 1,
    UMI_APPLICATION_ENTRY_UTILITY = 2,
    UMI_APPLICATION_ENTRY_SYSTEM = 3,
    UMI_APPLICATION_ENTRY_SERVICE = 4
} UmiApplicationEntryKind;

typedef struct UmiApplicationPresentation {
    uint32_t structure_size;
    const char *application_id;
    const char *icon_resource_id;
    const char *default_layout_id;
    const char *taskbar_group;
    UmiApplicationEntryKind entry_kind;
    bool pinned_by_default;
    bool visible_when_unavailable;
} UmiApplicationPresentation;

size_t umi_application_presentation_count(void);
const UmiApplicationPresentation *umi_application_presentation_at(size_t index);
const UmiApplicationPresentation *umi_application_presentation_find(
    const char *application_id);
UmiStatus umi_application_presentation_validate(
    const UmiApplicationPresentation *presentation);
UmiStatus umi_application_presentation_catalogue_validate(void);
const char *umi_application_entry_kind_text(UmiApplicationEntryKind kind);

#ifdef __cplusplus
}
#endif

#endif
