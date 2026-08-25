/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/timeline_qt6.cpp
 *
 * PURPOSE:
 *   Render multi-track timelines for video, music, animation and replay applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/timeline.h"
#include <cstring>

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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.timeline", "Umicom.Ui.Workstation.Timeline", "QGraphicsView", UMI_QT6_NATIVE_GRAPHICS_VIEW,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_HIGH_DENSITY | UMI_QT6_CAP_MEDIA
};

extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_timeline_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


extern "C" UmiQt6WidgetHandle umi_qt6_ws_timeline_create(const UmiWsTimelineModel *timeline) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *view=new QGraphicsView(); auto *scene=new QGraphicsScene(view); view->setScene(scene); const double track_h=44.0; const double px_per_ms=0.05;
    if (timeline != nullptr) { for (size_t i=0U;i<timeline->track_count;++i) { const double y=static_cast<double>(i)*track_h; scene->addText(QString::fromUtf8(timeline->tracks[i].label))->setPos(0.0,y); scene->addLine(120.0,y+track_h-2.0,1200.0,y+track_h-2.0,QPen(QColor(80,90,105))); }
      for (size_t i=0U;i<timeline->clip_count;++i) { size_t ti=0U; for (;ti<timeline->track_count;++ti) if (std::strcmp(timeline->clips[i].track_id,timeline->tracks[ti].track_id)==0) break; const double x=120.0+static_cast<double>(timeline->clips[i].start_ms)*px_per_ms; const double w=std::max(8.0,static_cast<double>(timeline->clips[i].duration_ms)*px_per_ms); scene->addRect(x,static_cast<double>(ti)*track_h+4.0,w,track_h-10.0,QPen(QColor(90,120,180)),QBrush(QColor(55,85,145))); } }
    view->setObjectName(QStringLiteral("umicomTimeline")); return view;
#else
    (void)timeline; return nullptr;
#endif
}

