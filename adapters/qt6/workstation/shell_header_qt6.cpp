/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/shell_header_qt6.cpp
 *
 * PURPOSE:
 *   Implement the shell header qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/shell_header_qt6.cpp
 *
 * PURPOSE:
 *   Render the universal workstation shell header for Studio, Trader, media, design and system applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/shell_header.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.shell_header", "Umicom.Ui.Workstation.ShellHeader", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 ws shell header descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_shell_header_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws shell header from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_shell_header_create(const UmiQt6RenderRequest *request) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    QWidget *parent = request != nullptr ? static_cast<QWidget *>(request->parent) : nullptr;
    auto *root = new QWidget(parent); auto *row = new QHBoxLayout(root); auto *titles = new QVBoxLayout();
    auto *title = new QLabel(QString::fromUtf8(request != nullptr && request->title != nullptr ? request->title : "Umicom Workstation"), root);
    title->setObjectName(QStringLiteral("umicomWorkstationTitle")); titles->addWidget(title);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != nullptr && request->subtitle != nullptr && request->subtitle[0] != '\0') titles->addWidget(new QLabel(QString::fromUtf8(request->subtitle), root));
    row->addLayout(titles, 1); root->setObjectName(QStringLiteral("umicomWorkstationHeader")); return root;
#else
    (void)request; return nullptr;
#endif
}

