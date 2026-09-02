/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/workspace_strip_qt6.cpp
 *
 * PURPOSE:
 *   Implement the workspace strip qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/workspace_strip_qt6.cpp
 *
 * PURPOSE:
 *   Render saved workspace/layout switching from the canonical workspace-strip model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/workspace_strip.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.workspace_strip", "Umicom.Ui.Workstation.WorkspaceStrip", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_DOCK | UMI_QT6_CAP_FLOAT
};

/*
 * Provide the qt6 ws workspace strip descriptor operation used by this module and its
 * client applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_workspace_strip_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws workspace strip from caller-provided values so later operations
 * receive a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *root = new QWidget(); auto *row = new QHBoxLayout(root); row->setContentsMargins(2,2,2,2); row->setSpacing(3);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip != nullptr) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (size_t i = 0U; i < strip->count; ++i) {
            auto *button = new QToolButton(root);
            button->setText(QString::fromUtf8(strip->entries[i].label));
            button->setCheckable(true);
            button->setChecked(i == strip->active_index);
            row->addWidget(button);
        }
    }
    row->addStretch(1);
    root->setObjectName(QStringLiteral("umicomWorkspaceStrip"));
    return root;
#else
    (void)strip; return nullptr;
#endif
}

