/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/chart_qt6.cpp
 *
 * PURPOSE:
 *   Implement the chart qt6 behaviour used by its public contract and client
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
 * File: adapters/qt6/workstation/chart_qt6.cpp
 *
 * PURPOSE:
 *   Render a reusable chart viewport for trading, observability and analytics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/chart.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.chart", "Umicom.Ui.Workstation.ChartSurface", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_HIGH_DENSITY
};

/*
 * Provide the qt6 ws chart descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_chart_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
class UmiQt6ChartWidget final : public QWidget {
public: explicit UmiQt6ChartWidget(const UmiWsChartSurface *surface):surface_(surface){setMinimumSize(240,160);}
protected: void paintEvent(QPaintEvent *event) override { (void)event; QPainter p(this); p.fillRect(rect(),QColor(15,22,30)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (surface_==nullptr || surface_->show_grid) { p.setPen(QColor(50,62,78)); /* Visit each bounded item once so every record receives the same rule. */ for(int x=0;x<width();x+=48)p.drawLine(x,0,x,height()); /* Visit each bounded item once so every record receives the same rule. */ for(int y=0;y<height();y+=36)p.drawLine(0,y,width(),y); } p.setRenderHint(QPainter::Antialiasing,true); p.setPen(QPen(QColor(66,190,145),2.0)); QPainterPath path; path.moveTo(0.0,height()*0.72); path.cubicTo(width()*0.25,height()*0.60,width()*0.55,height()*0.48,width(),height()*0.28); p.drawPath(path); }
private: const UmiWsChartSurface *surface_;
};
#endif
/*
 * Initialise qt6 ws chart from caller-provided values so later operations receive a known
 * state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_chart_create(const UmiWsChartSurface *surface) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *widget=new UmiQt6ChartWidget(surface); widget->setObjectName(QStringLiteral("umicomChartSurface")); widget->setToolTip(QString::fromUtf8(surface != nullptr ? surface->title : "Chart")); return widget;
#else
    (void)surface; return nullptr;
#endif
}

