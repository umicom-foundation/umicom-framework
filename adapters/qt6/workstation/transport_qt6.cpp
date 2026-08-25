/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/transport_qt6.cpp
 *
 * PURPOSE:
 *   Render media/replay transport state and playback controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/transport.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.transport", "Umicom.Ui.Workstation.Transport", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MEDIA
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_transport_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_transport_create(const UmiWsTransportModel *transport) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *root = new QWidget();
    auto *row = new QHBoxLayout(root);
    auto *back = new QPushButton(QStringLiteral("|<"), root);
    auto *play = new QPushButton(transport != nullptr && transport->state == UMI_WS_TRANSPORT_PLAYING ? QStringLiteral("Pause") : QStringLiteral("Play"), root);
    auto *forward = new QPushButton(QStringLiteral(">|"), root);
    auto *slider = new QSlider(Qt::Horizontal, root);
    slider->setRange(0, 1000);
    if (transport != nullptr && transport->duration_ms > 0) {
        const double fraction = static_cast<double>(transport->position_ms) / static_cast<double>(transport->duration_ms);
        slider->setValue(static_cast<int>(std::clamp(fraction * 1000.0, 0.0, 1000.0)));
    }
    row->addWidget(back);
    row->addWidget(play);
    row->addWidget(forward);
    row->addWidget(slider, 1);
    root->setObjectName(QStringLiteral("umicomTransport"));
    return root;
#else
    (void)transport; return nullptr;
#endif
}

