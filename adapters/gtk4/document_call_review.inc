/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_call_review.inc
 * PURPOSE: Present copied caller and callee destinations through the captured document navigation owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus CompletionReviewShowCalls(GtkWidget *window, CompletionReview *review)
{
    UmiGtk4Adapter *owner = CompletionReviewOwner(window, review);
    if (owner == NULL || review->calls == NULL)
        return UMI_STATUS_CANCELLED;
    UmiStatus status = UmiDocumentSourceRequestCheck(owner->edit_coordinator, review->source);
    size_t selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(review->choice));
    UmiLanguageCallRelation relation;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCallResultAt(review->calls, selected, &relation);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t generation = review->generation;
    GPtrArray *labels = g_ptr_array_new_with_free_func(g_free);
    GString *details = g_string_new(NULL);
    g_string_append_printf(details, "%s: %s\nPrepared symbol: %s\n%s\n\nRelated symbol: %s\n%s\n%s\n",
                           review->call_direction == UMI_LANGUAGE_CALL_INCOMING ? "Caller" : "Callee",
                           relation.related.name, relation.root.name, relation.root.location.uri,
                           relation.related.name, relation.related.location.uri, relation.related.detail);
    if (relation.related.deprecated)
        g_string_append(details, "Marked deprecated by the server.\n");
    /* Copy every label and description before publishing a widget or model.
     * A host observer may retire the result during a GTK notification. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i <= relation.call_sites; ++i)
    {
        UmiLanguageSourceLocation location;
        status = UmiLanguageCallResultLocation(review->calls, selected, i, &location);
        if (status != UMI_STATUS_OK)
            break;
        g_ptr_array_add(
            labels, g_strdup_printf("%s %zu: %s %llu:%llu", i == 0U ? "Symbol" : "Call site", i, location.uri,
                                    (unsigned long long)location.selection.start.line + 1ULL,
                                    (unsigned long long)location.selection.start.utf16_column + 1ULL));
        g_string_append_printf(details,
                               "\n%s %zu\n%s\n%llu:%llu to %llu:%llu (UTF-16 columns; end excluded)\n",
                               i == 0U ? "Symbol" : "Call site", i, location.uri,
                               (unsigned long long)location.selection.start.line + 1ULL,
                               (unsigned long long)location.selection.start.utf16_column + 1ULL,
                               (unsigned long long)location.selection.end.line + 1ULL,
                               (unsigned long long)location.selection.end.utf16_column + 1ULL);
    }
    if (relation.call_sites == 0U)
        g_string_append(details, "\nThe server supplied no individual call sites for this relationship.\n");
    g_ptr_array_add(labels, NULL);
    GtkStringList *model = gtk_string_list_new((const char *const *)labels->pdata);
    g_ptr_array_free(labels, TRUE);
    if (status == UMI_STATUS_OK && CompletionReviewOwner(window, review) != NULL &&
        review->generation == generation)
        gtk_drop_down_set_model(GTK_DROP_DOWN(review->call_location), G_LIST_MODEL(model));
    if (status == UMI_STATUS_OK && CompletionReviewOwner(window, review) != NULL &&
        review->generation == generation)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(review->call_location), 0U);
    if (status == UMI_STATUS_OK && CompletionReviewOwner(window, review) != NULL &&
        review->generation == generation)
        gtk_widget_set_sensitive(review->call_location, TRUE);
    g_object_unref(model);
    GtkWidget *view = g_object_ref_sink(gtk_text_view_new());
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), details->str, -1);
    g_string_free(details, TRUE);
    (void)umi_gtk4_automation_tag_widget(view, "document.calls.details");
    gtk_accessible_update_property(GTK_ACCESSIBLE(view), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Call relationship and source locations", -1);
    if (status == UMI_STATUS_OK && CompletionReviewOwner(window, review) != NULL &&
        review->generation == generation)
        gtk_box_append(GTK_BOX(review->comparison), view);
    g_object_unref(view);
    if (CompletionReviewOwner(window, review) == NULL || review->generation != generation)
        status = UMI_STATUS_CANCELLED;
    if (CompletionReviewOwner(window, review) != NULL && review->generation == generation)
        gtk_widget_set_sensitive(review->preview, status == UMI_STATUS_OK);
    return status;
}
static void CompletionReviewCallLocationChanged(GObject *control, GParamSpec *property, gpointer data)
{
    (void)control;
    (void)property;
    GtkWidget *window = g_object_ref(data);
    CompletionReview *review = CompletionReviewState(window);
    UmiGtk4Adapter *owner = CompletionReviewOwner(window, review);
    if (owner != NULL && review->call_direction != 0 && !review->rendering && review->pending == NULL &&
        review->calls != NULL)
    {
        UmiStatus status = UmiDocumentSourceRequestCheck(owner->edit_coordinator, review->source);
        UmiLanguageSourceLocation location;
        if (status == UMI_STATUS_OK)
            status = UmiLanguageCallResultLocation(
                review->calls, gtk_drop_down_get_selected(GTK_DROP_DOWN(review->choice)),
                gtk_drop_down_get_selected(GTK_DROP_DOWN(review->call_location)), &location);
        gtk_widget_set_sensitive(review->preview, status == UMI_STATUS_OK);
    }
    g_object_unref(window);
}
