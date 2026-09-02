/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/data_grid_qt6.cpp
 *
 * PURPOSE:
 *   Implement the data grid qt6 behaviour used by its public contract and client
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
 * File: adapters/qt6/workstation/data_grid_qt6.cpp
 *
 * PURPOSE:
 *   Render high-density semantic data-grid columns and row capacity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/data_grid.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.data_grid", "Umicom.Ui.Workstation.DataGrid", "QTableWidget", UMI_QT6_NATIVE_TABLE,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_HIGH_DENSITY
};

/*
 * Provide the qt6 ws data grid descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_data_grid_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


/*
 * Initialise qt6 ws data grid from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_data_grid_create(const UmiWsDataGridSurface *grid) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *table = new QTableWidget();
    const int columns = grid != nullptr ? static_cast<int>(grid->column_count) : 0;
    const int rows = grid != nullptr ? static_cast<int>(std::min<size_t>(grid->row_count, 10000U)) : 0;
    table->setColumnCount(columns);
    table->setRowCount(rows);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid != nullptr) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (size_t i = 0U; i < grid->column_count; ++i) {
            table->setHorizontalHeaderItem(static_cast<int>(i), new QTableWidgetItem(QString::fromUtf8(grid->columns[i].label)));
            table->setColumnWidth(static_cast<int>(i), grid->columns[i].width);
        }
    }
    table->setAlternatingRowColors(grid != nullptr && grid->striped);
    table->setObjectName(QStringLiteral("umicomDataGrid"));
    return table;
#else
    (void)grid; return nullptr;
#endif
}

