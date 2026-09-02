/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/inspector_qt6.cpp
 *
 * PURPOSE:
 *   Implement the inspector qt6 behaviour used by its public contract and client
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
 * File: adapters/qt6/workstation/inspector_qt6.cpp
 *
 * PURPOSE:
 *   Render generic editable/read-only property inspection state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/inspector.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.inspector", "Umicom.Ui.Workstation.Inspector", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

/*
 * Provide the qt6 ws inspector descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_inspector_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws inspector from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_inspector_create(const UmiWsInspectorModel *model) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *root = new QWidget();
    auto *form = new QFormLayout(root);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != nullptr) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (size_t i = 0U; i < model->count; ++i) {
            QWidget *value = nullptr;
            /* Apply this branch only when its contract condition is satisfied. */
            if (model->properties[i].editable) {
                value = new QLineEdit(QString::fromUtf8(model->properties[i].value), root);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                value = new QLabel(QString::fromUtf8(model->properties[i].value), root);
            }
            form->addRow(QString::fromUtf8(model->properties[i].label), value);
        }
    }
    root->setObjectName(QStringLiteral("umicomInspector"));
    return root;
#else
    (void)model; return nullptr;
#endif
}

