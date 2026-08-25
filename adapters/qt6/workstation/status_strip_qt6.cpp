/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/status_strip_qt6.cpp
 *
 * PURPOSE:
 *   Render workstation status items with severity and left/right alignment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/status_strip.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.status_strip", "Umicom.Ui.Workstation.StatusStrip", "QStatusBar", UMI_QT6_NATIVE_STATUS_BAR,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_status_strip_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_status_strip_create(const UmiWsStatusStrip *strip) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *status = new QStatusBar();
    if (strip != nullptr) {
        for (size_t i = 0U; i < strip->count; ++i) {
            auto *label = new QLabel(QString::fromUtf8(strip->items[i].text), status);
            label->setProperty("umicomSeverity", static_cast<int>(strip->items[i].severity));
            if (strip->items[i].align_right) {
                status->addPermanentWidget(label);
            } else {
                status->addWidget(label);
            }
        }
    }
    status->setObjectName(QStringLiteral("umicomStatusStrip"));
    return status;
#else
    (void)strip; return nullptr;
#endif
}

