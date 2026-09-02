/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/media_canvas_qt6.cpp
 *
 * PURPOSE:
 *   Implement the media canvas qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/media_canvas_qt6.cpp
 *
 * PURPOSE:
 *   Render media viewport, waveform and zoomable design-canvas surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/media_canvas.h"

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.media_canvas", "Umicom.Ui.Workstation.MediaCanvas", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MEDIA | UMI_QT6_CAP_CANVAS
};

/*
 * Provide the qt6 ws media canvas descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_media_canvas_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
/*
 * Provide the qt6 media canvas widget operation used by this module and its client
 * applications.
 */
class UmiQt6MediaCanvasWidget final : public QWidget { public: enum Mode{Media,Waveform,Canvas}; explicit UmiQt6MediaCanvasWidget(Mode m,const void *model):mode_(m),model_(model){setMinimumSize(240,160);} protected: void paintEvent(QPaintEvent *event) override {(void)event;QPainter p(this);p.fillRect(rect(),QColor(14,19,26));p.setRenderHint(QPainter::Antialiasing,true);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(mode_==Media){const auto *v=static_cast<const UmiWsMediaViewport *>(model_);p.setPen(QColor(80,105,150));p.drawRect(rect().adjusted(width()/12,height()/12,-width()/12,-height()/12));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v!=nullptr&&v->show_safe_area)p.drawRect(rect().adjusted(width()/7,height()/7,-width()/7,-height()/7));}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(mode_==Waveform){const auto *w=static_cast<const UmiWsWaveformSurface *>(model_);p.setPen(QPen(QColor(70,190,150),1.0));QPainterPath path;path.moveTo(0.0,height()*0.5);const double scale=w!=nullptr?w->vertical_scale:1.0;/* Visit each bounded item once so every record receives the same rule. */ for(int x=0;x<width();x+=4){const double amp=static_cast<double>((x/4)%11-5)*scale*2.5;path.lineTo(x,height()*0.5+amp);}p.drawPath(path);}/* Use this fallback path when the earlier condition does not apply. */ else{const auto *c=static_cast<const UmiWsCanvasSurface *>(model_);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==nullptr||c->show_grid){const int step=c!=nullptr&&c->grid_size>0?c->grid_size:24;p.setPen(QColor(65,75,95));/* Visit each bounded item once so every record receives the same rule. */ for(int x=0;x<width();x+=step)p.drawLine(x,0,x,height());/* Visit each bounded item once so every record receives the same rule. */ for(int y=0;y<height();y+=step)p.drawLine(0,y,width(),y);}}} private:Mode mode_;const void *model_;};
#endif
/*
 * Initialise qt6 ws media viewport from caller-provided values so later operations receive
 * a known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_media_viewport_create(const UmiWsMediaViewport *viewport) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *w=new UmiQt6MediaCanvasWidget(UmiQt6MediaCanvasWidget::Media,viewport);w->setObjectName(QStringLiteral("umicomMediaViewport"));return w;
#else
    (void)viewport; return nullptr;
#endif
}
/*
 * Initialise qt6 ws waveform from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_waveform_create(const UmiWsWaveformSurface *waveform) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *w=new UmiQt6MediaCanvasWidget(UmiQt6MediaCanvasWidget::Waveform,waveform);w->setObjectName(QStringLiteral("umicomWaveform"));return w;
#else
    (void)waveform; return nullptr;
#endif
}
/*
 * Initialise qt6 ws canvas from caller-provided values so later operations receive a known
 * state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_canvas_create(const UmiWsCanvasSurface *canvas) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *w=new UmiQt6MediaCanvasWidget(UmiQt6MediaCanvasWidget::Canvas,canvas);w->setObjectName(QStringLiteral("umicomDesignCanvas"));return w;
#else
    (void)canvas; return nullptr;
#endif
}

