/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/types.h
 *
 * PURPOSE:
 *   Define shared toolkit-neutral design-system values, limits and utility operations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TYPES_H
#define INCLUDE_UMICOM_UI_DESIGN_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/types.h"
#include "umicom/ui/components/component.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/types.h"
#include "umicom/ui/components/component.h"

#define UMI_DESIGN_ID_CAPACITY UMI_UI_ID_CAPACITY
#define UMI_DESIGN_NAME_CAPACITY 96U
#define UMI_DESIGN_TEXT_CAPACITY UMI_UI_TEXT_CAPACITY
#define UMI_DESIGN_MAX_ITEMS 128U
#define UMI_DESIGN_MAX_TOKENS 256U
#define UMI_DESIGN_MAX_VALUES 32U
#define UMI_DESIGN_MAX_STATES 16U
#define UMI_DESIGN_MAX_BREAKPOINTS 16U

typedef enum UmiDesignValueKind {
    UMI_DESIGN_VALUE_NONE = 0,
    UMI_DESIGN_VALUE_COLOR = 1,
    UMI_DESIGN_VALUE_NUMBER = 2,
    UMI_DESIGN_VALUE_INTEGER = 3,
    UMI_DESIGN_VALUE_TEXT = 4,
    UMI_DESIGN_VALUE_LENGTH = 5,
    UMI_DESIGN_VALUE_DURATION = 6
} UmiDesignValueKind;

typedef enum UmiDesignUnit {
    UMI_DESIGN_UNIT_NONE = 0,
    UMI_DESIGN_UNIT_PX = 1,
    UMI_DESIGN_UNIT_DP = 2,
    UMI_DESIGN_UNIT_SP = 3,
    UMI_DESIGN_UNIT_PERCENT = 4
} UmiDesignUnit;

typedef enum UmiDesignThemeMode {
    UMI_DESIGN_THEME_LIGHT = 1,
    UMI_DESIGN_THEME_DARK = 2,
    UMI_DESIGN_THEME_HIGH_CONTRAST = 3
} UmiDesignThemeMode;

typedef struct UmiDesignLength { double value; UmiDesignUnit unit; } UmiDesignLength;
typedef struct UmiDesignRgba { double red; double green; double blue; double alpha; } UmiDesignRgba;

/* Copy bounded design metadata and reject silent truncation. */
UmiStatus umi_design_copy_text(char *destination, size_t capacity, const char *source);
/* Return one only for finite design numeric inputs. */
int umi_design_number_valid(double value);
/* Clamp a finite design value to the requested inclusive range. */
UmiStatus umi_design_clamp(double value, double minimum, double maximum, double *out_value);
/* Return a deterministic FNV-1a identifier fingerprint for design caches. */
uint64_t umi_design_hash_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif
