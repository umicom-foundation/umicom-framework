/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/database_maintenance.h
 *
 * PURPOSE:
 *   Shared native access to checked SQLite inspection, backup and recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DATABASE_MAINTENANCE_H
#define UMICOM_UI_GTK4_DATABASE_MAINTENANCE_H
#include "umicom/data/backup.h"
#include "umicom/data/sqlite_health.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDatabaseMaintenanceOperation {
  UMI_DATABASE_MAINTENANCE_CHECK = 0,
  UMI_DATABASE_MAINTENANCE_BACKUP,
  UMI_DATABASE_MAINTENANCE_RECOVER
} UmiDatabaseMaintenanceOperation;
typedef UmiStatus (*UmiDatabaseMaintenanceAuthorize)(UmiDatabaseMaintenanceOperation operation,
                                                     const char *source, const char *destination,
                                                     void *context);
typedef struct UmiDatabaseMaintenanceGtk4Options {
  UmiDatabaseMaintenanceAuthorize authorize;
  void *authorize_context;
} UmiDatabaseMaintenanceGtk4Options;
typedef struct UmiDatabaseMaintenanceGtk4State {
  UmiStatus status;
  UmiDatabaseMaintenanceOperation operation;
  bool busy, retired, has_result;
  char source[1024], destination[1024];
  UmiSqliteHealthReport health;
  UmiDataBackupReport copy;
} UmiDatabaseMaintenanceGtk4State;
/* All entry points run on the GTK owner thread. The floating widget contains
 * an existing-source chooser, a new-destination chooser and explicit Check,
 * Create backup and Recover actions. Authorization is required for each action
 * and runs before any I/O. Its borrowed context must outlive the widget.
 * Re-entrance is BUSY; input changes or retirement during authorization cancel
 * admission. Inputs are copied before invoking application code. */
UmiStatus UmiDatabaseMaintenanceGtk4New(const UmiDatabaseMaintenanceGtk4Options *options,
                                        GtkWidget **out);
/* Returns admission status. The operation-finished(int UmiStatus) signal reports
 * completion. CHECK ignores destination; other actions require a new absolute
 * local path. A worker opens the source read-only and closes it on that worker.
 * CHECK success means the query ran; inspect health.sound separately.
 * Once admitted, copies may create files even if inputs change or the widget
 * retires. Their actual result and captured paths remain in State; retirement
 * never disguises a completed write as a cancellation. CHECK results instead
 * become CANCELLED when inputs change or the widget retires. Keep the GTK main
 * context running until completion. There is no mid-SQLite cancellation.
 * Retirement preserves connected operation-finished handlers. GObject disposal
 * disconnects handlers: an owner retaining a disposed widget must inspect State
 * after busy clears rather than expect a signal. The admitted copy still runs.
 * Recover creates a separate database and never switches the application's
 * current store. These actions do not authenticate data or verify domain rules. */
UmiStatus UmiDatabaseMaintenanceGtk4Run(GtkWidget *widget,
                                        UmiDatabaseMaintenanceOperation operation,
                                        const char *source, const char *destination);
UmiStatus UmiDatabaseMaintenanceGtk4StateRead(GtkWidget *widget,
                                              UmiDatabaseMaintenanceGtk4State *out);
bool UmiDatabaseMaintenanceGtk4CanClose(GtkWidget *widget);
void UmiDatabaseMaintenanceGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
