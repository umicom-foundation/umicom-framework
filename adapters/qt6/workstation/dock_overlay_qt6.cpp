/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/dock_overlay_qt6.cpp
 *
 * PURPOSE:
 *   Render docking targets and drop previews without modifying the Framework layout graph directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/dock_overlay.h"

#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
#include <algorithm>
#include <QBrush>
#include <QByteArray>
#include <QFrame>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPen>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QString>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#endif

static const UmiQt6SurfaceDescriptor UMI_QT6_WS_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.dock_overlay", "Umicom.Ui.Workstation.DockPreview", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_dock_overlay_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_dock_overlay_create(UmiQt6WidgetHandle content) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *root = new QWidget();
    auto *layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    if (content != nullptr) {
        layout->addWidget(static_cast<QWidget *>(content));
    }
    root->setObjectName(QStringLiteral("umicomDockOverlay"));
    return root;
#else
    (void)content; return nullptr;
#endif
}
extern "C" UmiStatus umi_qt6_ws_dock_overlay_set_preview(UmiQt6WidgetHandle overlay, const UmiWsDockPreview *preview, const char *label) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    if (overlay == nullptr || preview == nullptr) return UMI_STATUS_INVALID_ARGUMENT;
    auto *root = static_cast<QWidget *>(overlay);
    auto *indicator = root->findChild<QLabel *>(QStringLiteral("umicomDockPreviewLabel"));
    if (indicator == nullptr) {
        indicator = new QLabel(root);
        indicator->setObjectName(QStringLiteral("umicomDockPreviewLabel"));
    }
    indicator->setText(QString::fromUtf8(label != nullptr ? label : "Dock"));
    indicator->setGeometry(preview->bounds.x, preview->bounds.y, preview->bounds.width, preview->bounds.height);
    indicator->setVisible(preview->visible);
    indicator->setWindowOpacity(std::clamp(preview->opacity, 0.0, 1.0));
    return UMI_STATUS_OK;
#else
    (void)overlay; (void)preview; (void)label; return UMI_STATUS_UNAVAILABLE;
#endif
}

