/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/workstation/node_graph_qt6.cpp
 *
 * PURPOSE:
 *   Implement the node graph qt6 behaviour used by its public contract and
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
 * File: adapters/qt6/workstation/node_graph_qt6.cpp
 *
 * PURPOSE:
 *   Render generic workflow/compositor/AI/audio/system node graphs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/workstation/node_graph.h"
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
    UMI_QT6_ADAPTER_API_VERSION, "qt6.ws.node_graph", "Umicom.Ui.Workstation.NodeGraph", "QWidget", UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY | UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_HIGH_DENSITY | UMI_QT6_CAP_CANVAS
};

/*
 * Provide the qt6 ws node graph descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_ws_node_graph_descriptor(void) { return &UMI_QT6_WS_DESCRIPTOR; }


#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
class UmiQt6NodeGraphWidget final : public QWidget {
public: explicit UmiQt6NodeGraphWidget(const UmiWsNodeGraphSurface *graph):graph_(graph){setMinimumSize(260,180);}
protected: void paintEvent(QPaintEvent *event) override { (void)event; QPainter p(this); p.fillRect(rect(),QColor(16,23,32)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph_==nullptr)return; p.setRenderHint(QPainter::Antialiasing,true); p.setPen(QPen(QColor(100,110,140),1.5)); /* Visit each bounded item once so every record receives the same rule. */ for(size_t e=0U;e<graph_->edge_count;++e){const UmiWsGraphNode *a=nullptr,*b=nullptr; /* Visit each bounded item once so every record receives the same rule. */ for(size_t n=0U;n<graph_->node_count;++n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(std::strcmp(graph_->nodes[n].node_id,graph_->edges[e].source_id)==0)a=&graph_->nodes[n]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(std::strcmp(graph_->nodes[n].node_id,graph_->edges[e].target_id)==0)b=&graph_->nodes[n];} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(a!=nullptr&&b!=nullptr)p.drawLine(QPointF(a->x,a->y),QPointF(b->x,b->y));} p.setBrush(QColor(55,85,145)); p.setPen(QColor(125,155,220)); /* Visit each bounded item once so every record receives the same rule. */ for(size_t n=0U;n<graph_->node_count;++n){QRectF r(graph_->nodes[n].x-42.0,graph_->nodes[n].y-18.0,84.0,36.0);p.drawRoundedRect(r,4.0,4.0);p.drawText(r,Qt::AlignCenter,QString::fromUtf8(graph_->nodes[n].label));}}
private: const UmiWsNodeGraphSurface *graph_;
};
#endif
/*
 * Initialise qt6 ws node graph from caller-provided values so later operations receive a
 * known state.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_ws_node_graph_create(const UmiWsNodeGraphSurface *graph) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    auto *widget=new UmiQt6NodeGraphWidget(graph); widget->setObjectName(QStringLiteral("umicomNodeGraph")); return widget;
#else
    (void)graph; return nullptr;
#endif
}

