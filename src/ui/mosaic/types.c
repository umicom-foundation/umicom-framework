/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/types.c
 *
 * PURPOSE:
 *   Define stable types shared by panel catalogues, layout editing, docking and cross-application mosaic workspaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/types.h"
#include <string.h>

UmiStatus umi_ui_mosaic_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

int umi_ui_mosaic_id_is_valid(const char *identifier) {
    size_t i;
    if (identifier == NULL || identifier[0] == '\0') return 0;
    for (i = 0U; identifier[i] != '\0'; ++i) {
        const char c = identifier[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return 0;
        if (i + 1U >= UMI_UI_MOSAIC_ID_CAPACITY) return 0;
    }
    return 1;
}

const char *umi_ui_mosaic_application_name(UmiUiMosaicApplication application) {
    switch (application) {
        case UMI_UI_MOSAIC_APP_FRAMEWORK: return "Framework";
        case UMI_UI_MOSAIC_APP_DESK: return "Desk";
        case UMI_UI_MOSAIC_APP_STUDIO: return "Studio";
        case UMI_UI_MOSAIC_APP_TRADER: return "Trader";
        case UMI_UI_MOSAIC_APP_TMS: return "TMS";
        case UMI_UI_MOSAIC_APP_BANK: return "Bank";
        case UMI_UI_MOSAIC_APP_LLM: return "LLM";
        case UMI_UI_MOSAIC_APP_ACCOUNTANT: return "Accountant";
        case UMI_UI_MOSAIC_APP_COMMODITY: return "Commodity Exchange";
        case UMI_UI_MOSAIC_APP_MUSIC: return "Music Studio";
        case UMI_UI_MOSAIC_APP_VIDEO: return "Video Studio";
        case UMI_UI_MOSAIC_APP_OS: return "Umicom OS";
        default: return "Unknown";
    }
}

const char *umi_ui_mosaic_dock_zone_name(UmiUiMosaicDockZone zone) {
    switch (zone) {
        case UMI_UI_MOSAIC_DOCK_LEFT: return "left";
        case UMI_UI_MOSAIC_DOCK_RIGHT: return "right";
        case UMI_UI_MOSAIC_DOCK_TOP: return "top";
        case UMI_UI_MOSAIC_DOCK_BOTTOM: return "bottom";
        case UMI_UI_MOSAIC_DOCK_CENTRE: return "centre";
        case UMI_UI_MOSAIC_DOCK_FLOAT: return "float";
        default: return "unknown";
    }
}
