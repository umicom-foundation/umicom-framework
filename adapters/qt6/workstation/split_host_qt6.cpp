/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/split_host_qt6.cpp
 *
 * PURPOSE:
 *   Render semantic split regions through Qt6 splitter geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/split_host.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.split_host", "Umicom.Ui.Workstation.SplitRegion", "QSplitter", UMI_QT6_NATIVE_SPLITTER,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_split_host_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_split_host_create(const UmiWsSplitRegion *split, UmiQt6WidgetHandle first, UmiQt6WidgetHandle second) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    const Qt::Orientation orientation = split != nullptr && split->orientation == UMI_UI_VERTICAL ? Qt::Vertical : Qt::Horizontal;
    auto *host = new QSplitter(orientation); if (first != nullptr) host->addWidget(static_cast<QWidget *>(first)); if (second != nullptr) host->addWidget(static_cast<QWidget *>(second));
    const double ratio = split != nullptr ? std::clamp(split->ratio, 0.05, 0.95) : 0.5; host->setSizes({static_cast<int>(ratio * 1000.0), static_cast<int>((1.0-ratio)*1000.0)});
    host->setChildrenCollapsible(false); host->setObjectName(QStringLiteral("umicomSplitHost")); return host;
#else
    (void)split; (void)first; (void)second; return nullptr;
#endif
}

