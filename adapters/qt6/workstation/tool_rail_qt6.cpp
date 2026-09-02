/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/tool_rail_qt6.cpp
 *
 * PURPOSE:
 *   Implement the tool rail qt6 behaviour used by its public contract and client
 *   applications.
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
 * File: adapters/qt6/workstation/tool_rail_qt6.cpp
 *
 * PURPOSE:
 *   Render reusable activity/tool rails for professional workstation applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/tool_rail.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.tool_rail", "Umicom.Ui.Workstation.ToolRail", "QToolBar", UMI_QT6_NATIVE_TOOL_BAR,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

/*
 * Provide the qt6 ws tool rail descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_tool_rail_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws tool rail from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_tool_rail_create(const UmiQt6RenderRequest *request) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    QWidget *parent = request != nullptr ? static_cast<QWidget *>(request->parent) : nullptr; auto *rail = new QToolBar(parent); rail->setOrientation(Qt::Vertical); rail->setMovable(false); rail->setFloatable(false); rail->setObjectName(QStringLiteral("umicomToolRail")); return rail;
#else
    (void)request; return nullptr;
#endif
}

