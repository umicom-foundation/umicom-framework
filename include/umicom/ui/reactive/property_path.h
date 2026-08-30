/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/property_path.h
 *
 * PURPOSE:
 *   Parse and validate dotted property paths used by declarative bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPERTY_PATH_H
#define UMICOM_UI_REACTIVE_PROPERTY_PATH_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactivePropertyPath {
    char text[UMI_UI_REACTIVE_PATH_CAPACITY];
    size_t segment_count;
} UmiUiReactivePropertyPath;
UmiStatus umi_ui_reactive_property_path_parse(UmiUiReactivePropertyPath *item, const char *text);
int umi_ui_reactive_property_path_valid(const UmiUiReactivePropertyPath *item);
#ifdef __cplusplus
}
#endif
#endif
