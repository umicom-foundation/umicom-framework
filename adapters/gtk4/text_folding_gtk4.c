/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/text_folding_gtk4.c
 * PURPOSE: Project local fold ranges as owned GTK presentation tags while retaining complete source bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_folding.h"
#include "umicom/editor/workbench/folding_projection.h"

/* Tag-table removal replaces iterator-based clearing so callbacks cannot
 * invalidate pending clear iterators. The earlier implementation remains
 * available for engineering review. */
#if 0
typedef struct TextFolds {
    UmiEditorWbFoldingProjection projection;
    GtkTextTag *tag;
    uint64_t generation;
    int applying,placing,clearing,attached;
} TextFolds;
static TextFolds *TextFoldsState(GtkTextBuffer *buffer)
{return g_object_get_data(G_OBJECT(buffer),"umicom-text-folds");}
static void TextFoldsFree(gpointer data)
{
    TextFolds *folds=data;g_clear_object(&folds->tag);g_free(folds);
}
static void TextFoldsClear(GtkTextBuffer *buffer,TextFolds *folds)
{
    if(folds==NULL || folds->clearing) return;
    folds->clearing=1;
    if(folds->generation!=UINT64_MAX) ++folds->generation;
    umi_editor_wb_folding_projection_init(&folds->projection);
    if(folds->attached) {
        GtkTextIter begin,end;gtk_text_buffer_get_bounds(buffer,&begin,&end);
        gtk_text_buffer_remove_tag(buffer,folds->tag,&begin,&end);
    }
    folds->clearing=0;
}
static void TextFoldsChanged(GtkTextBuffer *buffer,gpointer data)
{
    (void)data;TextFolds *folds=TextFoldsState(buffer);
    if(folds==NULL) return;
    /* A host can change text from an apply-tag observer. Stop that pending
     * emission before its default handler can use iterators from old text. */
    if(folds->applying) g_signal_stop_emission_by_name(buffer,"apply-tag");
    TextFoldsClear(buffer,folds);
}
static int TextFoldsTouchesSelection(GtkTextBuffer *buffer,TextFolds *folds)
{
    GtkTextIter begin,end;
    if(!gtk_text_buffer_get_selection_bounds(buffer,&begin,&end)) {
        gtk_text_buffer_get_iter_at_mark(buffer,&begin,gtk_text_buffer_get_insert(buffer));
        return gtk_text_iter_has_tag(&begin,folds->tag);
    }
    int first=gtk_text_iter_get_offset(&begin),last=gtk_text_iter_get_offset(&end);
    int lines=gtk_text_buffer_get_line_count(buffer);
    for(size_t i=0U;i<folds->projection.count;++i) {
        const UmiEditorWbRange *range=&folds->projection.folds[i];
        GtkTextIter hidden,end_hidden;
        if(range->start.line>=(uint32_t)lines) continue;
        gtk_text_buffer_get_iter_at_line(buffer,&hidden,(int)range->start.line);
        if(range->end.line>=(uint32_t)lines) gtk_text_buffer_get_end_iter(buffer,&end_hidden);
        else gtk_text_buffer_get_iter_at_line(buffer,&end_hidden,(int)range->end.line);
        if(first<gtk_text_iter_get_offset(&end_hidden) && last>gtk_text_iter_get_offset(&hidden)) return 1;
    }
    return 0;
}
static void TextFoldsMarkSet(GtkTextBuffer *buffer,GtkTextIter *location,GtkTextMark *mark,gpointer data)
{
    (void)location;(void)data;TextFolds *folds=TextFoldsState(buffer);
    if(folds==NULL || folds->clearing || folds->applying || folds->placing || folds->projection.count==0U) return;
    if(mark!=gtk_text_buffer_get_insert(buffer) && mark!=gtk_text_buffer_get_selection_bound(buffer)) return;
    /* A programmatic search or source navigation must never leave its result
     * hidden. Reveal the full projection when the selection intersects a fold. */
    if(TextFoldsTouchesSelection(buffer,folds)) TextFoldsClear(buffer,folds);
}
static TextFolds *TextFoldsEnsure(GtkTextBuffer *buffer)
{
    TextFolds *folds=TextFoldsState(buffer);
    if(folds!=NULL) return folds;
    folds=g_new0(TextFolds,1);folds->generation=1U;
    umi_editor_wb_folding_projection_init(&folds->projection);
    /* An unnamed tag cannot collide with a theme or application's public tag.
     * The buffer's table and this state each keep their own reference. */
    folds->tag=gtk_text_tag_new(NULL);g_object_set(folds->tag,"invisible",TRUE,NULL);
    /* Start observing before publishing the tag: table observers can edit
     * the buffer during construction. No tag removal is attempted until the
     * table owns it, but those edits still retire the captured generation. */
    g_object_set_data_full(G_OBJECT(buffer),"umicom-text-folds",folds,TextFoldsFree);
    g_signal_connect(buffer,"changed",G_CALLBACK(TextFoldsChanged),NULL);
    g_signal_connect(buffer,"mark-set",G_CALLBACK(TextFoldsMarkSet),NULL);
    if(!gtk_text_tag_table_add(gtk_text_buffer_get_tag_table(buffer),folds->tag)) return NULL;
    folds->attached=1;
    return folds;
}
UmiStatus UmiGtk4TextFoldSelection(GtkTextView *view)
{
    if(!GTK_IS_TEXT_VIEW(view)) return UMI_STATUS_INVALID_ARGUMENT;
    g_object_ref(view);GtkTextBuffer *buffer=g_object_ref(gtk_text_view_get_buffer(view));
    GtkTextIter begin,end,hidden,hidden_end;UmiStatus status=UMI_STATUS_INVALID_ARGUMENT;
    if(!gtk_text_buffer_get_selection_bounds(buffer,&begin,&end)) goto done;
    int first=gtk_text_iter_get_line(&begin),last=gtk_text_iter_get_line(&end);
    if(gtk_text_iter_starts_line(&end) && last>first) --last;
    if(last<=first) goto done;
    int header_offset=gtk_text_iter_get_offset(&begin);
    int creating=TextFoldsState(buffer)==NULL;
    TextFolds *folds=TextFoldsEnsure(buffer);
    if(folds==NULL) {status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    if(creating && folds->generation!=1U) {status=UMI_STATUS_INVALID_STATE;goto done;}
    if(folds->applying || folds->placing || folds->clearing) {status=UMI_STATUS_BUSY;goto done;}
    if(folds->generation==UINT64_MAX) {status=UMI_STATUS_CAPACITY_EXCEEDED;goto done;}
    uint32_t first_line=(uint32_t)first+1U,last_line=(uint32_t)last+1U;
    UmiEditorWbFoldingProjection candidate=folds->projection;int duplicate=0;
    for(size_t i=0U;i<candidate.count;++i)
        if(candidate.folds[i].start.line==first_line && candidate.folds[i].end.line==last_line) duplicate=1;
    status=duplicate?UMI_STATUS_OK:umi_editor_wb_folding_projection_add(&candidate,first_line,last_line);
    if(status!=UMI_STATUS_OK) goto done;
    gtk_text_buffer_get_iter_at_line(buffer,&hidden,first+1);
    if(last+1>=gtk_text_buffer_get_line_count(buffer)) gtk_text_buffer_get_end_iter(buffer,&hidden_end);
    else gtk_text_buffer_get_iter_at_line(buffer,&hidden_end,last+1);
    uint64_t generation=folds->generation;
    folds->applying=1;
    gtk_text_buffer_apply_tag(buffer,folds->tag,&hidden,&hidden_end);
    folds->applying=0;
    if(folds->generation!=generation || gtk_text_view_get_buffer(view)!=buffer) {status=UMI_STATUS_INVALID_STATE;goto done;}
    folds->projection=candidate;
    /* Use a fresh iterator after callbacks. Moving the caret to the visible
     * header does not create a text edit or add a document Undo operation. */
    gtk_text_buffer_get_iter_at_offset(buffer,&begin,header_offset);
    folds->placing=1;gtk_text_buffer_place_cursor(buffer,&begin);folds->placing=0;
    if(folds->generation!=generation || gtk_text_view_get_buffer(view)!=buffer) status=UMI_STATUS_INVALID_STATE;
    else if(TextFoldsTouchesSelection(buffer,folds)) {TextFoldsClear(buffer,folds);status=UMI_STATUS_INVALID_STATE;}
done:
    g_object_unref(buffer);g_object_unref(view);return status;
}
UmiStatus UmiGtk4TextFoldClear(GtkTextView *view)
{
    if(!GTK_IS_TEXT_VIEW(view)) return UMI_STATUS_INVALID_ARGUMENT;
    GtkTextBuffer *buffer=g_object_ref(gtk_text_view_get_buffer(view));TextFolds *folds=TextFoldsState(buffer);
    if(folds!=NULL && (folds->applying || folds->placing || folds->clearing)) {g_object_unref(buffer);return UMI_STATUS_BUSY;}
    TextFoldsClear(buffer,folds);g_object_unref(buffer);return UMI_STATUS_OK;
}
size_t UmiGtk4TextFoldCount(GtkTextView *view)
{
    if(!GTK_IS_TEXT_VIEW(view)) return 0U;
    TextFolds *folds=TextFoldsState(gtk_text_view_get_buffer(view));return folds==NULL?0U:folds->projection.count;
}
int UmiGtk4TextFoldLineHidden(GtkTextView *view,uint32_t line)
{
    if(!GTK_IS_TEXT_VIEW(view) || line==0U) return 0;
    TextFolds *folds=TextFoldsState(gtk_text_view_get_buffer(view));
    return folds==NULL?0:umi_editor_wb_folding_projection_line_hidden(&folds->projection,line);
}

#endif

typedef struct TextFolds
{
    UmiEditorWbFoldingProjection projection;
    GtkTextTag *tag;
    uint64_t generation;
    int applying, placing, clearing, attaching, attached;
} TextFolds;
static TextFolds *TextFoldsState(GtkTextBuffer *buffer)
{
    return g_object_get_data(G_OBJECT(buffer), "umicom-text-folds");
}
static void TextFoldsFree(gpointer data)
{
    TextFolds *folds = data;
    g_clear_object(&folds->tag);
    g_free(folds);
}
static void TextFoldsRetire(TextFolds *folds)
{
    if (folds->generation != UINT64_MAX)
        ++folds->generation;
    umi_editor_wb_folding_projection_init(&folds->projection);
}
static void TextFoldsTagRemoved(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    (void)table;
    GtkTextBuffer *buffer = data;
    TextFolds *folds = TextFoldsState(buffer);
    if (folds == NULL || folds->tag != tag)
        return;
    folds->attached = 0;
    if (folds->applying)
        g_signal_stop_emission_by_name(buffer, "apply-tag");
    if (!folds->clearing)
        TextFoldsRetire(folds);
}
static void TextFoldsClear(GtkTextBuffer *buffer, TextFolds *folds)
{
    if (folds == NULL || folds->clearing)
        return;
    folds->clearing = 1;
    TextFoldsRetire(folds);
    /* Removing our tag from its table removes its ranges from every buffer
     * that uses that table. No GtkTextIter survives a host notification, so
     * even an observer that edits source cannot leave stale iterators behind.
     * The unnamed tag belongs only to this buffer's folding service. */
    if (folds->attached)
    {
        folds->attached = 0;
        gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table(buffer), folds->tag);
    }
    folds->clearing = 0;
}
static void TextFoldsChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)data;
    TextFolds *folds = TextFoldsState(buffer);
    if (folds == NULL)
        return;
    /* An apply-tag observer may edit source before GTK's default handler.
     * Stop that emission so its old iterators cannot reach the handler. */
    if (folds->applying)
        g_signal_stop_emission_by_name(buffer, "apply-tag");
    TextFoldsClear(buffer, folds);
}
static int TextFoldsTouchesSelection(GtkTextBuffer *buffer, TextFolds *folds)
{
    GtkTextIter begin, end;
    if (!gtk_text_buffer_get_selection_bounds(buffer, &begin, &end))
    {
        gtk_text_buffer_get_iter_at_mark(buffer, &begin, gtk_text_buffer_get_insert(buffer));
        return gtk_text_iter_has_tag(&begin, folds->tag);
    }
    int first = gtk_text_iter_get_offset(&begin), last = gtk_text_iter_get_offset(&end);
    int lines = gtk_text_buffer_get_line_count(buffer);
    for (size_t i = 0U; i < folds->projection.count; ++i)
    {
        const UmiEditorWbRange *range = &folds->projection.folds[i];
        GtkTextIter hidden, end_hidden;
        if (range->start.line >= (uint32_t)lines)
            continue;
        gtk_text_buffer_get_iter_at_line(buffer, &hidden, (int)range->start.line);
        if (range->end.line >= (uint32_t)lines)
            gtk_text_buffer_get_end_iter(buffer, &end_hidden);
        else
            gtk_text_buffer_get_iter_at_line(buffer, &end_hidden, (int)range->end.line);
        if (first < gtk_text_iter_get_offset(&end_hidden) && last > gtk_text_iter_get_offset(&hidden))
            return 1;
    }
    return 0;
}
static void TextFoldsMarkSet(GtkTextBuffer *buffer, GtkTextIter *location, GtkTextMark *mark, gpointer data)
{
    (void)location;
    (void)data;
    TextFolds *folds = TextFoldsState(buffer);
    if (folds == NULL || folds->clearing || folds->applying || folds->placing || folds->attaching ||
        folds->projection.count == 0U)
        return;
    if (mark != gtk_text_buffer_get_insert(buffer) && mark != gtk_text_buffer_get_selection_bound(buffer))
        return;
    /* Search, debugger navigation and selection commands must expose their
     * destination. Reveal all manual folds if they contain that destination. */
    if (TextFoldsTouchesSelection(buffer, folds))
        TextFoldsClear(buffer, folds);
}
static TextFolds *TextFoldsEnsure(GtkTextBuffer *buffer)
{
    TextFolds *folds = TextFoldsState(buffer);
    if (folds != NULL)
        return folds;
    folds = g_new0(TextFolds, 1);
    folds->generation = 1U;
    umi_editor_wb_folding_projection_init(&folds->projection);
    folds->tag = gtk_text_tag_new(NULL);
    g_object_set(folds->tag, "invisible", TRUE, NULL);
    g_object_set_data_full(G_OBJECT(buffer), "umicom-text-folds", folds, TextFoldsFree);
    g_signal_connect(buffer, "changed", G_CALLBACK(TextFoldsChanged), NULL);
    g_signal_connect(buffer, "mark-set", G_CALLBACK(TextFoldsMarkSet), NULL);
    /* The table may be shared with other buffers. This weak connection ends
     * with our buffer and does not create an ownership cycle. */
    g_signal_connect_object(gtk_text_buffer_get_tag_table(buffer), "tag-removed",
                            G_CALLBACK(TextFoldsTagRemoved), buffer, 0);
    return folds;
}
static int TextFoldsSelectionCurrent(GtkTextView *view, GtkTextBuffer *buffer, int start, int end)
{
    GtkTextIter begin, last;
    return gtk_text_view_get_buffer(view) == buffer &&
           gtk_text_buffer_get_selection_bounds(buffer, &begin, &last) &&
           gtk_text_iter_get_offset(&begin) == start && gtk_text_iter_get_offset(&last) == end;
}
UmiStatus UmiGtk4TextFoldSelection(GtkTextView *view)
{
    if (!GTK_IS_TEXT_VIEW(view))
        return UMI_STATUS_INVALID_ARGUMENT;
    g_object_ref(view);
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(view));
    GtkTextIter begin, end, hidden, hidden_end;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (!gtk_text_buffer_get_selection_bounds(buffer, &begin, &end))
        goto done;
    int first = gtk_text_iter_get_line(&begin), last = gtk_text_iter_get_line(&end);
    if (gtk_text_iter_starts_line(&end) && last > first)
        --last;
    if (last <= first)
        goto done;
    int header_offset = gtk_text_iter_get_offset(&begin), end_offset = gtk_text_iter_get_offset(&end);
    TextFolds *folds = TextFoldsEnsure(buffer);
    if (folds->applying || folds->placing || folds->clearing || folds->attaching)
    {
        status = UMI_STATUS_BUSY;
        goto done;
    }
    if (folds->generation == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    uint64_t generation = folds->generation;
    uint32_t first_line = (uint32_t)first + 1U, last_line = (uint32_t)last + 1U;
    UmiEditorWbFoldingProjection candidate = folds->projection;
    int duplicate = 0;
    for (size_t i = 0U; i < candidate.count; ++i)
        if (candidate.folds[i].start.line == first_line && candidate.folds[i].end.line == last_line)
            duplicate = 1;
    status =
        duplicate ? UMI_STATUS_OK : umi_editor_wb_folding_projection_add(&candidate, first_line, last_line);
    if (status != UMI_STATUS_OK)
        goto done;
    if (!folds->attached)
    {
        folds->attaching = 1;
        folds->attached = 1;
        if (!gtk_text_tag_table_add(gtk_text_buffer_get_tag_table(buffer), folds->tag))
            folds->attached = 0;
        folds->attaching = 0;
    }
    /* A host can navigate, edit or replace a buffer while the tag is added.
     * Validate the captured selection again before constructing fresh ranges. */
    if (!folds->attached || folds->generation != generation ||
        !TextFoldsSelectionCurrent(view, buffer, header_offset, end_offset))
        goto stale;
    gtk_text_buffer_get_iter_at_line(buffer, &hidden, first + 1);
    if (last + 1 >= gtk_text_buffer_get_line_count(buffer))
        gtk_text_buffer_get_end_iter(buffer, &hidden_end);
    else
        gtk_text_buffer_get_iter_at_line(buffer, &hidden_end, last + 1);
    folds->applying = 1;
    gtk_text_buffer_apply_tag(buffer, folds->tag, &hidden, &hidden_end);
    folds->applying = 0;
    if (folds->generation != generation ||
        !TextFoldsSelectionCurrent(view, buffer, header_offset, end_offset))
        goto stale;
    folds->projection = candidate;
    /* The caret moves to the visible header. No text edit, save or document
     * Undo entry is created by this presentation operation. */
    gtk_text_buffer_get_iter_at_offset(buffer, &begin, header_offset);
    folds->placing = 1;
    gtk_text_buffer_place_cursor(buffer, &begin);
    folds->placing = 0;
    if (folds->generation != generation || gtk_text_view_get_buffer(view) != buffer ||
        TextFoldsTouchesSelection(buffer, folds))
        goto stale;
    goto done;
stale:
    TextFoldsClear(buffer, folds);
    status = UMI_STATUS_INVALID_STATE;
done:
    g_object_unref(buffer);
    g_object_unref(view);
    return status;
}
UmiStatus UmiGtk4TextFoldClear(GtkTextView *view)
{
    if (!GTK_IS_TEXT_VIEW(view))
        return UMI_STATUS_INVALID_ARGUMENT;
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(view));
    TextFolds *folds = TextFoldsState(buffer);
    if (folds != NULL && (folds->applying || folds->placing || folds->clearing || folds->attaching))
    {
        g_object_unref(buffer);
        return UMI_STATUS_BUSY;
    }
    TextFoldsClear(buffer, folds);
    g_object_unref(buffer);
    return UMI_STATUS_OK;
}
size_t UmiGtk4TextFoldCount(GtkTextView *view)
{
    if (!GTK_IS_TEXT_VIEW(view))
        return 0U;
    TextFolds *folds = TextFoldsState(gtk_text_view_get_buffer(view));
    return folds == NULL ? 0U : folds->projection.count;
}
int UmiGtk4TextFoldLineHidden(GtkTextView *view, uint32_t line)
{
    if (!GTK_IS_TEXT_VIEW(view) || line == 0U)
        return 0;
    TextFolds *folds = TextFoldsState(gtk_text_view_get_buffer(view));
    return folds == NULL ? 0 : umi_editor_wb_folding_projection_line_hidden(&folds->projection, line);
}
