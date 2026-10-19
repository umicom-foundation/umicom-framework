/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_review_gtk4.c
 * Purpose: Exercise qualification panel ownership, filtering and paging with copied evidence.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/qualification_review.h"
#include <string.h>
#include <stdlib.h>
#include "umicom/distribution/runtime/qualification_archive.h"
#include "umicom/distribution/runtime/qualification_report.h"
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name); if (found != NULL) return found;
    }
    return NULL;
}
static bool LabelContains(GtkWidget *root, const char *text)
{
    if (GTK_IS_LABEL(root) && strstr(gtk_label_get_text(GTK_LABEL(root)), text) != NULL) return true;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child))
        if (LabelContains(child, text)) return true;
    return false;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    if (!gtk_init_check()) return 77;
    GtkWidget *panel = UmiGtk4QualificationReviewCreate(); g_object_ref_sink(panel);
    size_t offset = 99U, visible = 99U, total = 99U;
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK);
    REQUIRE(offset == 0U && visible == 0U && total == 0U);
    QualificationFixture f; REQUIRE(QualificationStart(&f, 130U, false) == 0);
    UmiQualificationView *view = NULL;
    REQUIRE(UmiQualificationViewCapture(f.run, &view) == UMI_STATUS_OK);
    if (strcmp(argv[1], "invalid") == 0) {
        GtkWidget *other = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(other);
        REQUIRE(UmiGtk4QualificationReviewSetView(other, &view) == UMI_STATUS_INVALID_ARGUMENT && view != NULL);
        REQUIRE(UmiGtk4QualificationReviewSetView(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        g_object_unref(other);
    }
    REQUIRE(UmiGtk4QualificationReviewSetView(panel, &view) == UMI_STATUS_OK && view == NULL);
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && visible == 64U);
    GtkWidget *filter = Find(panel, "qualification-filter"), *next = Find(panel, "qualification-next");
    REQUIRE(filter != NULL && next != NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), UMI_QUALIFICATION_VIEW_TESTS);
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && total == 130U && offset == 0U);
    g_signal_emit_by_name(next, "clicked");
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && offset == 64U && visible == 64U);
    g_signal_emit_by_name(next, "clicked");
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && offset == 128U && visible == 2U);
    REQUIRE(!gtk_widget_get_sensitive(next));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), UMI_QUALIFICATION_VIEW_PROBLEMS);
    REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && total == 0U && offset == 0U);
    REQUIRE(QualificationRecordAll(&f) == 0);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    REQUIRE(UmiQualificationViewCapture(f.run, &view) == UMI_STATUS_OK);
    REQUIRE(UmiGtk4QualificationReviewSetView(panel, &view) == UMI_STATUS_OK && view == NULL);
    if (strcmp(argv[1], "archived") == 0) {
        size_t capacity = 0U;
        REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &capacity) == UMI_STATUS_OK);
        char *report = malloc(capacity); REQUIRE(report != NULL);
        REQUIRE(UmiReleaseQualificationFormatReport(f.run, report, capacity, &capacity) == UMI_STATUS_OK);
        UmiQualificationArchiveReceipt receipt;
        REQUIRE(UmiQualificationArchiveParse(report, capacity - 1U, &view, &receipt) == UMI_STATUS_OK);
        free(report);
        REQUIRE(UmiGtk4QualificationReviewSetView(panel, &view) == UMI_STATUS_OK && view == NULL);
        REQUIRE(LabelContains(panel, "Saved scoped result"));
        REQUIRE(LabelContains(panel, "current source files and tools have not been checked"));
    }
    QualificationDestroy(&f);
    if (strcmp(argv[1], "lifetime") == 0) {
        /* A retained child cannot call freed presentation state after root dies. */
        g_object_ref(next); g_object_unref(panel);
        g_signal_emit_by_name(next, "clicked"); g_object_unref(next);
    } else {
        REQUIRE(UmiGtk4QualificationReviewSetView(panel, &view) == UMI_STATUS_OK);
        REQUIRE(UmiGtk4QualificationReviewReadPage(panel, &offset, &visible, &total) == UMI_STATUS_OK && total == 0U);
        g_object_unref(panel);
    }
    return 0;
}
