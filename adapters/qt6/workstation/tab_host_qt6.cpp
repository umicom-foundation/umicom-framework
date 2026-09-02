/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/tab_host_qt6.cpp
 *
 * PURPOSE:
 *   Implement the tab host qt6 behaviour used by its public contract and client
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
 * File: adapters/qt6/workstation/tab_host_qt6.cpp
 *
 * PURPOSE:
 *   Render semantic tab stacks through a reorderable Qt6 tab host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/tab_host.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.tab_host", "Umicom.Ui.Workstation.TabStack", "QTabWidget", UMI_QT6_NATIVE_TAB,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

/*
 * Provide the qt6 ws tab host descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_tab_host_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws tab host from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_tab_host_create(const UmiWsTabStack *stack) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *tabs = new QTabWidget(); tabs->setMovable(stack == nullptr || stack->allow_reorder); tabs->setTabsClosable(false); tabs->setDocumentMode(true);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack != nullptr && stack->count > 0U && stack->active_index < stack->count) tabs->setCurrentIndex(static_cast<int>(stack->active_index));
    tabs->setObjectName(QStringLiteral("umicomTabHost")); return tabs;
#else
    (void)stack; return nullptr;
#endif
}
/* Add qt6 ws tab host only after its inputs and available capacity have been checked. */
extern "C" UmiStatus umi_qt6_ws_tab_host_append(UmiQt6WidgetHandle host, const char *label, UmiQt6WidgetHandle child) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == nullptr || label == nullptr || child == nullptr) return UMI_STATUS_INVALID_ARGUMENT;
    auto *tabs = static_cast<QTabWidget *>(host); tabs->addTab(static_cast<QWidget *>(child), QString::fromUtf8(label)); return UMI_STATUS_OK;
#else
    (void)host; (void)label; (void)child; return UMI_STATUS_UNAVAILABLE;
#endif
}

