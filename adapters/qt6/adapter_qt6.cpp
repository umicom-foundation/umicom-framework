/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/qt6/adapter_qt6.cpp
 *
 * PURPOSE:
 *   Implement the adapter qt6 behaviour used by its public contract and client
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
 * File: adapters/qt6/adapter_qt6.cpp
 *
 * PURPOSE:
 *   Qt6 adapter ABI, surface descriptors, native widget handles and shared scaffold creation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/qt6/adapter.h"

#include <cstring>

#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
#include <QByteArray>
#include <QDialog>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QString>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#endif

static const UmiQt6SurfaceDescriptor UMI_QT6_ADAPTER_DESCRIPTOR = {
    UMI_QT6_ADAPTER_API_VERSION,
    "qt6.adapter",
    "Umicom.Ui.Adapter",
    "QWidget",
    UMI_QT6_NATIVE_WIDGET,
    UMI_QT6_CAP_FOCUS | UMI_QT6_CAP_KEYBOARD | UMI_QT6_CAP_ACCESSIBILITY |
        UMI_QT6_CAP_DENSITY | UMI_QT6_CAP_THEME | UMI_QT6_CAP_MULTI_MONITOR
};

/*
 * Provide the qt6 adapter descriptor operation used by this module and its client
 * applications.
 */
extern "C" const UmiQt6SurfaceDescriptor *umi_qt6_adapter_descriptor(void) {
    return &UMI_QT6_ADAPTER_DESCRIPTOR;
}

/* Provide the qt6 adapter info operation used by this module and its client applications. */
extern "C" UmiQt6AdapterInfo umi_qt6_adapter_info(void) {
    UmiQt6AdapterInfo info{};
    info.api_version = UMI_QT6_ADAPTER_API_VERSION;
    info.minimum_qt_major = 6U;
    info.minimum_qt_minor = 5U;
    info.minimum_qt_patch = 0U;
    info.capability_flags = UMI_QT6_ADAPTER_DESCRIPTOR.capability_flags;
    return info;
}

/*
 * Check that qt6 surface descriptor satisfies its contract before another service relies
 * on it.
 */
extern "C" UmiStatus umi_qt6_surface_descriptor_validate(const UmiQt6SurfaceDescriptor *descriptor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == nullptr) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (descriptor->api_version != UMI_QT6_ADAPTER_API_VERSION) return UMI_STATUS_INVALID_STATE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (descriptor->surface_id[0] == '\0' || descriptor->semantic_contract[0] == '\0' || descriptor->native_class[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->native_kind < UMI_QT6_NATIVE_WIDGET || descriptor->native_kind > UMI_QT6_NATIVE_TAB)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Provide the qt6 native kind text operation used by this module and its client
 * applications.
 */
extern "C" const char *umi_qt6_native_kind_text(UmiQt6NativeKind kind) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_QT6_NATIVE_WIDGET: return "widget";
        case UMI_QT6_NATIVE_MAIN_WINDOW: return "main-window";
        case UMI_QT6_NATIVE_DOCK_WIDGET: return "dock-widget";
        case UMI_QT6_NATIVE_TOOL_BAR: return "tool-bar";
        case UMI_QT6_NATIVE_STATUS_BAR: return "status-bar";
        case UMI_QT6_NATIVE_MENU_BAR: return "menu-bar";
        case UMI_QT6_NATIVE_DIALOG: return "dialog";
        case UMI_QT6_NATIVE_TREE: return "tree";
        case UMI_QT6_NATIVE_LIST: return "list";
        case UMI_QT6_NATIVE_LINE_EDIT: return "line-edit";
        case UMI_QT6_NATIVE_TEXT_EDIT: return "text-edit";
        case UMI_QT6_NATIVE_SPLITTER: return "splitter";
        case UMI_QT6_NATIVE_GRAPHICS_VIEW: return "graphics-view";
        case UMI_QT6_NATIVE_PROGRESS: return "progress";
        case UMI_QT6_NATIVE_STACKED: return "stacked";
        case UMI_QT6_NATIVE_TABLE: return "table";
        case UMI_QT6_NATIVE_TAB: return "tab";
        default: return "unknown";
    }
}

/*
 * Provide the qt6 create scaffold operation used by this module and its client
 * applications.
 */
extern "C" UmiQt6WidgetHandle umi_qt6_create_scaffold(const UmiQt6SurfaceDescriptor *descriptor,
                                                        const UmiQt6RenderRequest *request) {
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_qt6_surface_descriptor_validate(descriptor) != UMI_STATUS_OK) return nullptr;
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    QWidget *parent = request != nullptr ? static_cast<QWidget *>(request->parent) : nullptr;
    const QString title = QString::fromUtf8(request != nullptr && request->title != nullptr ? request->title : descriptor->semantic_contract);
    QWidget *widget = nullptr;
    /* Select the behaviour associated with the requested command or state value. */
    switch (descriptor->native_kind) {
        case UMI_QT6_NATIVE_MAIN_WINDOW: widget = new QMainWindow(parent); break;
        case UMI_QT6_NATIVE_DOCK_WIDGET: widget = new QDockWidget(title, parent); break;
        case UMI_QT6_NATIVE_TOOL_BAR: widget = new QToolBar(title, parent); break;
        case UMI_QT6_NATIVE_STATUS_BAR: widget = new QStatusBar(parent); break;
        case UMI_QT6_NATIVE_MENU_BAR: widget = new QMenuBar(parent); break;
        case UMI_QT6_NATIVE_DIALOG: widget = new QDialog(parent); break;
        case UMI_QT6_NATIVE_TREE: widget = new QTreeWidget(parent); break;
        case UMI_QT6_NATIVE_LIST: widget = new QListWidget(parent); break;
        case UMI_QT6_NATIVE_LINE_EDIT: widget = new QLineEdit(parent); break;
        case UMI_QT6_NATIVE_TEXT_EDIT: widget = new QPlainTextEdit(parent); break;
        case UMI_QT6_NATIVE_SPLITTER: widget = new QSplitter(parent); break;
        case UMI_QT6_NATIVE_GRAPHICS_VIEW: {
            auto *view = new QGraphicsView(parent);
            view->setScene(new QGraphicsScene(view));
            widget = view;
            break;
        }
        case UMI_QT6_NATIVE_PROGRESS: widget = new QProgressBar(parent); break;
        case UMI_QT6_NATIVE_STACKED: widget = new QStackedWidget(parent); break;
        case UMI_QT6_NATIVE_TABLE: widget = new QTableWidget(parent); break;
        case UMI_QT6_NATIVE_TAB: widget = new QTabWidget(parent); break;
        case UMI_QT6_NATIVE_WIDGET:
        default: {
            auto *container = new QWidget(parent);
            auto *layout = new QVBoxLayout(container);
            auto *heading = new QLabel(title, container);
            heading->setObjectName(QStringLiteral("umicomSurfaceTitle"));
            layout->addWidget(heading);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (request != nullptr && request->subtitle != nullptr && request->subtitle[0] != '\0') {
                auto *subtitle = new QLabel(QString::fromUtf8(request->subtitle), container);
                subtitle->setObjectName(QStringLiteral("umicomSurfaceSubtitle"));
                layout->addWidget(subtitle);
            }
            widget = container;
            break;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (widget != nullptr) {
        widget->setObjectName(QString::fromUtf8(descriptor->surface_id));
        widget->setProperty("umicomSemanticContract", QString::fromUtf8(descriptor->semantic_contract));
        widget->setProperty("umicomNativeKind", QString::fromLatin1(umi_qt6_native_kind_text(descriptor->native_kind)));
    }
    return static_cast<UmiQt6WidgetHandle>(widget);
#else
    (void)request;
    return nullptr;
#endif
}

/* Release or reset state held by qt6 widget so the same storage can be reused safely. */
extern "C" void umi_qt6_widget_destroy(UmiQt6WidgetHandle widget) {
#if defined(UMICOM_QT6_NATIVE) && UMICOM_QT6_NATIVE
    delete static_cast<QWidget *>(widget);
#else
    (void)widget;
#endif
}
