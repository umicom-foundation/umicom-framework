/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/accessibility.h
 *
 * PURPOSE:
 *   Keep reusable accessibility metadata with semantic components rather than individual product widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_ACCESSIBILITY_H
#define UMICOM_UI_COMPONENTS_ACCESSIBILITY_H
#include "umicom/base/status.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UI_ACCESSIBLE_TEXT_CAPACITY 256U
/**
 * Represent the ui component accessibility data shared with callers of this public
 * contract.
 */
typedef struct UmiUiComponentAccessibility { char name[UMI_UI_ACCESSIBLE_TEXT_CAPACITY]; char description[UMI_UI_ACCESSIBLE_TEXT_CAPACITY]; } UmiUiComponentAccessibility;
/**
 * Copy ui component accessibility into module-owned storage so callers keep ownership of
 * their input values.
 */
/** Publish both fields together or leave the complete value unchanged. NULL
 * text still means empty. Inputs may borrow either destination field, provided
 * each is readable through its terminator or UMI_UI_ACCESSIBLE_TEXT_CAPACITY.
 * No text is truncated; serialize access through the value owner. */
UmiStatus umi_ui_component_accessibility_set(UmiUiComponentAccessibility *value,const char *name,const char *description);

#ifdef __cplusplus
}
#endif
#endif
