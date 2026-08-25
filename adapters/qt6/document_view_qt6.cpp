/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/document_view_qt6.cpp
 *
 * PURPOSE:
 *   Render Framework document-view state as a Qt6 document surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/document_view.h"

static const UmiQt6SurfaceDescriptor UMI_QT6_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.document_view",
    "Umicom.Ui.DocumentView",
    "QPlainTextEdit",
    UMI_QT6_NATIVE_TEXT_EDIT,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_document_view_descriptor(void) {
    return &UMI_QT6_DESCRIPTOR;
}

extern "C" UmiQt6WidgetHandle umi_qt6_document_view_create(const UmiQt6RenderRequest *request) {
    return umi_qt6_create_scaffold(&UMI_QT6_DESCRIPTOR, request);
}
