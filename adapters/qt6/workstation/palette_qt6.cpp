/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/palette_qt6.cpp
 *
 * PURPOSE:
 *   Render searchable component/tool palettes for designers and editors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/palette.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.palette", "Umicom.Ui.Workstation.Palette", "QListWidget", UMI_QT6_NATIVE_LIST,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_palette_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_palette_create(const UmiWsPaletteModel *model) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *list = new QListWidget();
    if (model != nullptr) {
        for (size_t i = 0U; i < model->count; ++i) {
            auto *item = new QListWidgetItem(QString::fromUtf8(model->items[i].label), list);
            item->setData(Qt::UserRole, QString::fromUtf8(model->items[i].item_id));
            item->setToolTip(QString::fromUtf8(model->items[i].category));
        }
    }
    list->setObjectName(QStringLiteral("umicomPalette"));
    return list;
#else
    (void)model; return nullptr;
#endif
}

