/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/types.h
 *
 * PURPOSE:
 *   Define stable appearance identifiers, theme scopes, renderer kinds and bounded utility operations.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_TYPES_H
#define UMICOM_UI_APPEARANCE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/design/types.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPEARANCE_ID_CAPACITY 128U
#define UMI_APPEARANCE_TEXT_CAPACITY 256U
#define UMI_APPEARANCE_TOKEN_CAPACITY 128U
#define UMI_APPEARANCE_MAX_ITEMS 128U
#define UMI_APPEARANCE_MAX_FONTS 16U
#define UMI_APPEARANCE_MAX_ICONS 128U
#define UMI_APPEARANCE_MAX_FOCUS_ITEMS 128U

/**
 * List the named appearance scope values accepted by this public contract.
 */
typedef enum UmiAppearanceScope {
    UMI_APPEARANCE_SCOPE_SYSTEM = 1,
    UMI_APPEARANCE_SCOPE_APPLICATION = 2,
    UMI_APPEARANCE_SCOPE_WORKSPACE = 3,
    UMI_APPEARANCE_SCOPE_COMPONENT = 4
} UmiAppearanceScope;

/**
 * List the named appearance renderer kind values accepted by this public contract.
 */
typedef enum UmiAppearanceRendererKind {
    UMI_APPEARANCE_RENDERER_GTK4 = 1,
    UMI_APPEARANCE_RENDERER_QT6 = 2,
    UMI_APPEARANCE_RENDERER_NATIVE_WEB = 3,
    UMI_APPEARANCE_RENDERER_HEADLESS = 4
} UmiAppearanceRendererKind;

/**
 * List the named appearance input modality values accepted by this public contract.
 */
typedef enum UmiAppearanceInputModality {
    UMI_APPEARANCE_INPUT_POINTER = 1,
    UMI_APPEARANCE_INPUT_TOUCH = 2,
    UMI_APPEARANCE_INPUT_KEYBOARD = 3,
    UMI_APPEARANCE_INPUT_HYBRID = 4
} UmiAppearanceInputModality;

/* Copy bounded appearance metadata and reject silent truncation. */
UmiStatus umi_appearance_copy_text(char *destination, size_t capacity, const char *source);
/* Return one only for non-empty stable appearance identifiers. */
int umi_appearance_id_valid(const char *identifier);
/* Clamp a finite scale value to an inclusive production range. */
UmiStatus umi_appearance_clamp_scale(double value, double minimum, double maximum, double *out_value);

#ifdef __cplusplus
}
#endif
#endif
