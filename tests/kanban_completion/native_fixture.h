/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/native_fixture.h
 *
 * PURPOSE:
 *   Arrange visible native review controls with bounded dispatch and isolated private files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_COMPLETION_NATIVE_FIXTURE_H
#define UMICOM_TEST_COMPLETION_NATIVE_FIXTURE_H
#include "../build_log/fixture.h"
#include "../support/main_context.h"
#include "fixture.h"
#include "umicom/kanban/completion_storage.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/kanban_completion.h"
static inline GtkWidget *Control(GtkWidget *root, const char *suffix)
{
    char tag[96];
    int length = snprintf(tag, sizeof tag, "kanban.completion.%s", suffix);
    REQUIRE(length > 0 && (size_t)length < sizeof tag);
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, tag);
    REQUIRE(widget != NULL);
    return widget;
}
static inline void Field(GtkWidget *root, const char *suffix, const char *text)
{
    GtkWidget *widget = Control(root, suffix);
    REQUIRE(GTK_IS_EDITABLE(widget));
    gtk_editable_set_text(GTK_EDITABLE(widget), text);
}
static inline void ClickReview(GtkWidget *root, const char *suffix)
{
    GtkWidget *widget = Control(root, suffix);
    REQUIRE(GTK_IS_BUTTON(widget));
    g_signal_emit_by_name(widget, "clicked");
}
static inline void Approve(GtkWidget *root)
{
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
}
static inline void NativeDrain(void)
{
    for (unsigned i = 0U; i < 30U; ++i) {
        (void)UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 1000, 32U);
        g_usleep(1000U);
    }
}
static inline void NativeWait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiKanbanCompletionGtk4Pending(root) && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U);
        g_usleep(1000U);
    }
    REQUIRE(!UmiKanbanCompletionGtk4Pending(root));
    NativeDrain();
}
static inline void FillReview(GtkWidget *root, const char *path)
{
    char digest[65]; memset(digest, 'a', 64U); digest[64] = '\0';
    Field(root, "reviewer", "reviewer");
    Field(root, "source", "synthetic-source");
    Field(root, "configuration", "Debug/native");
    Field(root, "digest", digest);
    Field(root, "rationale", "Synthetic reviewed acceptance; not a real qualification result.");
    Field(root, "path", path);
}
static inline void ExpectColumn(UmiKanbanBoard *board, const char *id, const char *column)
{
    UmiKanbanCardSnapshot card;
    REQUIRE(umi_kanban_board_find(board, id, &card) == UMI_STATUS_OK);
    REQUIRE(!strcmp(card.column_id, column));
}
static inline GtkTextBuffer *Preview(GtkWidget *root)
{
    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(Control(root, "preview")));
}
#endif
