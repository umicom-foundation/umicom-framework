# Review a whole-document replacement

A replacement review lets someone inspect an edit before it happens. It owns
two complete copies of text: the current editor draft and the proposed result.
The draft can include typing that has not been saved. Creating a review does
not write a file, change the selection or add an Undo entry.

Include `umicom/document/replacement.h` and use the document coordinator that
already owns the editor. Keep calls on that coordinator's owner thread. Its
borrowed store and workbench must remain alive through Check and Apply.

1. Capture the intended document ID. Use the active working-copy snapshot for
   a toolbar command; do not look up the active tab again after confirmation.
2. Call `UmiDocumentCoordinatorPrepareReplacement` with that ID, the text to
   find and its replacement. Check the returned status. The output plan is
   cleared on failure. An empty replacement deletes matches; an empty search
   is invalid. Read-only documents cannot be prepared for editing.
3. Read `UmiDocumentReplacementPlanSummary` for the name, match count and byte
   sizes. `text_changes` tells you whether the result differs. A match can
   replace text with identical text. Zero matches is a successful review.
4. Read `UmiDocumentReplacementPlanTexts`. Display these exact buffers as
   read-only text. They belong to the plan; do not free or modify them. They
   remain valid until successful Apply or destruction. Later typing does not
   change these captured buffers.
5. Wait for an explicit choice. Cancel destroys the plan. Accept calls
   `UmiDocumentCoordinatorApplyReplacement` with the original coordinator.
   Apply checks the original document before editing it. Check performs the
   same validity checks without editing, but is not a reservation.
6. Destroy the plan on every exit path. A successful Apply consumes it, so
   it cannot be applied or read again. One Undo restores the reviewed draft.
   If that draft contained pending typing, its earlier text remains a separate
   history step, subject to the existing bounded history budget.

The complete console example is
[`replacement_review.c`](../examples/editor_workflow/replacement_review.c).
It prints two versions of a fixed Notes example, applies the proposal in
memory, then checks that Undo restores the draft. It deliberately demonstrates
acceptance without prompting and never saves a file. The learning-example
target is `umicom-replacement-review-example`.

## Matching and limits

This is literal matching in one document. `note` matches `note` and `NOTE`
because lowercase searches ignore ASCII letter case. `Note` requests exact
case because it contains an uppercase ASCII letter. Non-ASCII UTF-8 bytes
compare exactly; there is no Unicode case folding. Matches do not overlap.
For example, replacing `aa` with `b` in `aaaaa` produces `bba`.

The search and replacement must be valid NUL-terminated UTF-8 strings.
A draft containing invalid UTF-8 is refused with `INVALID_STATE`, without edits.
`$1`, brackets and backslashes are ordinary text. There are no regular
expressions, replacement templates or escape expansion in this API. It does
not implement workspace-wide replacement or symbol-aware refactoring.

The review uses the complete draft, including bytes beyond the short legacy
view preview. Each input and the proposed draft are limited by
`UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES` (currently 8 MiB). Preparation refuses an
oversized result before changing anything. The two captured texts and history
can require several times the document size in memory.

## When a review becomes stale

Changing draft bytes, editing and then restoring the same bytes, changing the
store or saved revision, renaming the target, changing its view identity, or
changing tracked external-file state invalidates the review. The same revision
number is not enough: Apply also checks captured text and identity. An ordinary
caret move or tab switch does not invalidate a whole-document review. Apply
targets the captured document and leaves the active tab alone.

On `INVALID_STATE`, discard the question and create a new review. On
`NOT_FOUND`, the target was closed or removed. On `PERMISSION_DENIED`, it is
read-only. Allocation and capacity errors leave the draft and history intact.
Do not automatically retry with fresh state: that would apply an edit the
person has not reviewed.

Saving is separate. Preparation and Apply never read or write the provider.
An external disk edit is detected by the existing external-change and save
workflows; this in-memory review is not a promise that the saved file is
unchanged. Keep the existing coordinator alive through Apply. Captured text
can still be read and destroyed after it closes, but must not be applied
through a different coordinator.

The older immediate `UmiDocumentCoordinatorReplaceAll` API remains available
with its existing behavior. Frontends that need user review should use the
plan API described here.

## Close a native review safely

GTK window removal can happen before final object destruction when another
component retains a reference. Include `umicom/ui/gtk4/window_lifecycle.h` in
the native frontend and call `UmiGtk4WatchWindowClosed` with the dialog and a
receiver object that owns the callback data. Release the plan in that callback
and clear its pointer, then make final cleanup tolerate the cleared pointer.

Use object-bound button signals and `UmiGtk4WindowIsOpen` before acting on a
retained control. A hidden, never-presented window is still open; a retained
destroyed window is not. Losing the receiver cancels its observation. An
already closed window invokes the callback immediately. Keep calls on GTK's
main thread and keep borrowed callback data alive until removal or receiver
finalisation. Studio's confirmation presenter uses this same Framework watch
for both replacement reviews and saved-file comparisons.
