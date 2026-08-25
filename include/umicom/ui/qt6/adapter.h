/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/adapter.h
 *
 * PURPOSE:
 *   Qt6 adapter ABI, surface descriptors, native widget handles and shared scaffold creation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ADAPTER_H
#define UMICOM_UI_QT6_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_QT6_ADAPTER_API_VERSION 1U
#define UMI_QT6_SURFACE_ID_CAPACITY 96U
#define UMI_QT6_CONTRACT_CAPACITY 128U
#define UMI_QT6_NATIVE_CLASS_CAPACITY 64U

typedef void *UmiQt6WidgetHandle;
typedef void *UmiQt6ActionHandle;

typedef enum UmiQt6NativeKind {
    UMI_QT6_NATIVE_WIDGET = 1,
    UMI_QT6_NATIVE_MAIN_WINDOW = 2,
    UMI_QT6_NATIVE_DOCK_WIDGET = 3,
    UMI_QT6_NATIVE_TOOL_BAR = 4,
    UMI_QT6_NATIVE_STATUS_BAR = 5,
    UMI_QT6_NATIVE_MENU_BAR = 6,
    UMI_QT6_NATIVE_DIALOG = 7,
    UMI_QT6_NATIVE_TREE = 8,
    UMI_QT6_NATIVE_LIST = 9,
    UMI_QT6_NATIVE_LINE_EDIT = 10,
    UMI_QT6_NATIVE_TEXT_EDIT = 11,
    UMI_QT6_NATIVE_SPLITTER = 12,
    UMI_QT6_NATIVE_GRAPHICS_VIEW = 13,
    UMI_QT6_NATIVE_PROGRESS = 14,
    UMI_QT6_NATIVE_STACKED = 15,
    UMI_QT6_NATIVE_TABLE = 16,
    UMI_QT6_NATIVE_TAB = 17
} UmiQt6NativeKind;

typedef enum UmiQt6SurfaceCapability {
    UMI_QT6_CAP_FOCUS = UINT64_C(1) << 0,
    UMI_QT6_CAP_KEYBOARD = UINT64_C(1) << 1,
    UMI_QT6_CAP_DOCK = UINT64_C(1) << 2,
    UMI_QT6_CAP_FLOAT = UINT64_C(1) << 3,
    UMI_QT6_CAP_CONTEXT = UINT64_C(1) << 4,
    UMI_QT6_CAP_ACCESSIBILITY = UINT64_C(1) << 5,
    UMI_QT6_CAP_DENSITY = UINT64_C(1) << 6,
    UMI_QT6_CAP_THEME = UINT64_C(1) << 7,
    UMI_QT6_CAP_MULTI_MONITOR = UINT64_C(1) << 8,
    UMI_QT6_CAP_HIGH_DENSITY = UINT64_C(1) << 9,
    UMI_QT6_CAP_MEDIA = UINT64_C(1) << 10,
    UMI_QT6_CAP_CANVAS = UINT64_C(1) << 11
} UmiQt6SurfaceCapability;

typedef struct UmiQt6SurfaceDescriptor {
    uint32_t api_version;
    char surface_id[UMI_QT6_SURFACE_ID_CAPACITY];
    char semantic_contract[UMI_QT6_CONTRACT_CAPACITY];
    char native_class[UMI_QT6_NATIVE_CLASS_CAPACITY];
    UmiQt6NativeKind native_kind;
    uint64_t capability_flags;
} UmiQt6SurfaceDescriptor;

typedef struct UmiQt6RenderRequest {
    const void *model;
    const char *model_contract;
    const char *title;
    const char *subtitle;
    UmiQt6WidgetHandle parent;
    uint64_t flags;
} UmiQt6RenderRequest;

typedef struct UmiQt6AdapterInfo {
    uint32_t api_version;
    uint32_t minimum_qt_major;
    uint32_t minimum_qt_minor;
    uint32_t minimum_qt_patch;
    uint64_t capability_flags;
} UmiQt6AdapterInfo;

const UmiQt6SurfaceDescriptor *umi_qt6_adapter_descriptor(void);
UmiQt6AdapterInfo umi_qt6_adapter_info(void);
UmiStatus umi_qt6_surface_descriptor_validate(const UmiQt6SurfaceDescriptor *descriptor);
const char *umi_qt6_native_kind_text(UmiQt6NativeKind kind);
UmiQt6WidgetHandle umi_qt6_create_scaffold(const UmiQt6SurfaceDescriptor *descriptor,
                                            const UmiQt6RenderRequest *request);
void umi_qt6_widget_destroy(UmiQt6WidgetHandle widget);

#ifdef __cplusplus
}
#endif
#endif
