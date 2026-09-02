/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/command_bar_qt6.cpp
 *
 * PURPOSE:
 *   Implement the command bar qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/command_bar_qt6.cpp
 *
 * PURPOSE:
 *   Render and parse the unified command/search centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/command_bar.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.command_bar", "Umicom.Ui.Workstation.CommandBar", "QLineEdit", UMI_QT6_NATIVE_LINE_EDIT,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 ws command bar descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_command_bar_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws command bar from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_command_bar_create(const char *placeholder) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *entry=new QLineEdit(); entry->setPlaceholderText(QString::fromUtf8(placeholder != nullptr ? placeholder : "Search commands, panels, layouts, settings, symbols and AI...")); entry->setClearButtonEnabled(true); entry->setObjectName(QStringLiteral("umicomCommandBar")); return entry;
#else
    (void)placeholder; return nullptr;
#endif
}
/*
 * Provide the qt6 ws command bar query operation used by this module and its client
 * applications.
 */
extern "C" UmiStatus umi_qt6_ws_command_bar_query(UmiQt6WidgetHandle entry, UmiWsCommandBarQuery *out_query) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == nullptr || out_query == nullptr) return UMI_STATUS_INVALID_ARGUMENT;
    const QByteArray utf8 = static_cast<QLineEdit *>(entry)->text().toUtf8();
    return umi_ws_command_bar_parse(utf8.constData(), out_query);
#else
    (void)entry; (void)out_query; return UMI_STATUS_UNAVAILABLE;
#endif
}

