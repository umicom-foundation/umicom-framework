/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/asset_browser_qt6.cpp
 *
 * PURPOSE:
 *   Implement the asset browser qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/asset_browser_qt6.cpp
 *
 * PURPOSE:
 *   Render reusable image/video/audio/model/material/project asset browsing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/asset_browser.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.asset_browser", "Umicom.Ui.Workstation.AssetBrowser", "QListWidget", UMI_QT6_NATIVE_LIST,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MEDIA
};

/*
 * Provide the qt6 ws asset browser descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_asset_browser_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws asset browser from caller-provided values so later operations receive
 * a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_asset_browser_create(const UmiWsAssetBrowserModel *model) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *list = new QListWidget();
    list->setViewMode(QListView::IconMode);
    list->setResizeMode(QListView::Adjust);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != nullptr) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (size_t i = 0U; i < model->count; ++i) {
            auto *item = new QListWidgetItem(QString::fromUtf8(model->assets[i].label), list);
            item->setData(Qt::UserRole, QString::fromUtf8(model->assets[i].uri));
            item->setToolTip(QStringLiteral("%1 bytes").arg(static_cast<qulonglong>(model->assets[i].size_bytes)));
        }
    }
    list->setObjectName(QStringLiteral("umicomAssetBrowser"));
    return list;
#else
    (void)model; return nullptr;
#endif
}

