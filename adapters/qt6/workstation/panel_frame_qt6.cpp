/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/panel_frame_qt6.cpp
 *
 * PURPOSE:
 *   Implement the panel frame qt6 behaviour used by its public contract and
 *   client applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/panel_frame_qt6.cpp
 *
 * PURPOSE:
 *   Render Framework panel chrome around arbitrary Qt6 child content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/panel_frame.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.panel_frame", "Umicom.Ui.Workstation.PanelChrome", "QFrame", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

/*
 * Provide the qt6 ws panel frame descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_panel_frame_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws panel frame from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_panel_frame_create(const UmiWsPanelChrome *chrome, UmiQt6WidgetHandle child) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *frame = new QFrame(); auto *layout = new QVBoxLayout(frame); auto *head = new QWidget(frame); auto *row = new QHBoxLayout(head);
    row->setContentsMargins(4, 2, 4, 2); auto *title = new QLabel(QString::fromUtf8(chrome != nullptr ? chrome->title : "Panel"), head); row->addWidget(title, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome != nullptr && chrome->badge[0] != '\0') row->addWidget(new QLabel(QString::fromUtf8(chrome->badge), head));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome != nullptr && chrome->show_pin) row->addWidget(new QToolButton(head));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome != nullptr && chrome->show_menu) row->addWidget(new QToolButton(head));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (chrome != nullptr && chrome->show_close) row->addWidget(new QToolButton(head));
    layout->addWidget(head);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (child != nullptr) {
        layout->addWidget(static_cast<QWidget *>(child), 1);
    }
    frame->setObjectName(QStringLiteral("umicomPanelFrame"));
    return frame;
#else
    (void)chrome; (void)child; return nullptr;
#endif
}

