/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/learning_catalogue.h
 *
 * PURPOSE:
 *   Share an offline learning catalogue and verified text reader across products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_LEARNING_CATALOGUE_H
#define UMICOM_UI_GTK4_LEARNING_CATALOGUE_H
#include <gtk/gtk.h>
#include "umicom/browser/reading_document.h"
#include "umicom/teacher/learning_reader.h"
#include "umicom/teacher/learning_search.h"
#include "umicom/teacher/learning_help.h"
#include "umicom/teacher/learning_pack_changes.h"
#include "umicom/teacher/learning_progress_local.h"
#include "umicom/teacher/learning_practice.h"
#include "umicom/teacher/learning_library_choices.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLearningCatalogueGtkState {
    size_t pack_count;
    UmiLearningSearchInfo search;
    UmiLearningReaderSnapshot reader;
    UmiStatus last_status;
    int busy;
    int retired;
    int has_copy_result;
    int has_progress_result;
    int progress_write_blocked;
    size_t library_choice_count;
    int has_library_result;
    int has_reading_structure;
    unsigned int reading_zoom_percent;
} UmiLearningCatalogueGtkState;

/* Return a floating widget, or NULL on controller allocation failure. Call all
 * functions on the GTK owner thread. Keep the internal widget hierarchy intact.
 * host_version is the embedding product's compatibility version. No directory
 * is scanned, file read, project opened or command run during creation.
 * Earlier presentation policy, retained for review: the initial reader was text-only: HTML gets a bounded plain-text
 * preview; images and binary/invalid UTF-8 content receive explicit notices.
 * Rich illustrated rendering is a separate capability, not implied here.
 * Structured HTML now retains headings, code spacing, image descriptions and
 * explicit link metadata when admitted; the earlier text fallback remains.
 * Explicit IMAGE selections now use the optional bounded Media PNG service,
 * described by CopyIllustration below. Inline HTML images remain descriptions. */
GtkWidget *UmiLearningCatalogueGtk4Create(UmiVersion host_version);

/* Present a new catalogue window tied to parent and its GtkApplication.
 * Return a borrowed window, or NULL for invalid parent/allocation failure.
 * Closing either window retires the panel. No pack is loaded implicitly.
 * For application-backed windows, each accepted read holds the application
 * until GTK completion so closing the last window cannot strand cleanup.
 * Hosts without GtkApplication must drain PendingReads themselves.
 * Explicit g_application_quit ignores holds: drain accepted jobs before that
 * call. Practice-copy jobs follow the same hold and completion contract. */
GtkWindow *UmiLearningCatalogueGtk4Present(GtkWindow *parent, UmiVersion host_version);

/* Re-present one catalogue per live parent, retaining its admitted packs and
 * reading history. The parent holds a weak window reference, not ownership of
 * a closed window. A closed/retired catalogue is replaced. Different parents
 * remain independent. A different host version while the cached window is live
 * is refused with NULL; close it explicitly before changing that version.
 * Reentrant presentation, destroyed parents and callback-driven closure return
 * NULL. The returned handle is borrowed as for Present. No pack is autoloaded
 * or persisted across process restart. Call on the GTK owner thread. */
GtkWindow *UmiLearningCatalogueGtk4PresentShared(GtkWindow *parent, UmiVersion host_version);

/* Accept a copied absolute root and asynchronously load learning-pack.json.
 * One job is admitted per panel. Duplicate IDs preserve the existing pack and
 * return ALREADY_EXISTS through last_status. Loading additional packs retains
 * the current reader. A failed operation leaves earlier catalogue data intact.
 * OK means dispatched, not completed; inspect CopyState after busy becomes zero.
 * Native reads can block. There is no promise to interrupt an accepted read. */
UmiStatus UmiLearningCatalogueGtk4Load(GtkWidget *widget, const char *absolute_root);


/* A replacement is a reviewable candidate, not an automatic library update.
 * Prepare reads only the chosen manifest on a worker and keeps the current
 * shelf, search and reader intact. Same ID/product/locale and a newer revision
 * are required. OK means dispatched; inspect last_status after busy clears.
 * A failed preparation retains any earlier pending review. */
typedef struct UmiLearningCatalogueReplacement {
    uint64_t review_serial;
    uint64_t shelf_generation;
    UmiLearningPackInfo previous;
    UmiLearningPackInfo proposed;
    UmiLearningPackChangeSummary changes;
} UmiLearningCatalogueReplacement;
UmiStatus UmiLearningCatalogueGtk4PrepareReplacement(GtkWidget *widget,
    const char *absolute_root);
/* Copy metadata for the last successful pending preparation. NOT_FOUND means
 * none is pending. Refusal preserves output. Metadata does not authenticate a
 * publisher or verify all resource bytes; each later open still verifies them. */
UmiStatus UmiLearningCatalogueGtk4CopyReplacement(GtkWidget *widget,
    UmiLearningCatalogueReplacement *out_review);
/* Copy one resource metadata change from that exact pending review. A stale
 * serial returns BUSY; an out-of-range index returns NOT_FOUND. Counts are in
 * CopyReplacement's changes value. No files are opened by either copy API. */
UmiStatus UmiLearningCatalogueGtk4ReplacementChangeAt(GtkWidget *widget,
    uint64_t review_serial, size_t index, UmiLearningResourceChange *out_change);
/* Explicitly accept the copied review serial. A different serial or changed
 * shelf generation returns BUSY, preserving the pending review. Search for the
 * current query is prepared before committing the candidate shelf. Allocation
 * or query failure preserves the previous shelf, search, reader and review.
 * Existing readers keep their admitted revision and history. Explicitly open a
 * result to begin reading the new revision; no disk files are overwritten. */
UmiStatus UmiLearningCatalogueGtk4AcceptReplacement(GtkWidget *widget, uint64_t review_serial);
/* Discard only the matching pending review; never remove an admitted pack. */
UmiStatus UmiLearningCatalogueGtk4DiscardReplacement(GtkWidget *widget, uint64_t review_serial);

/* Search copied metadata synchronously, then explicitly open a displayed index
 * asynchronously. Indices belong to the current result set, not persistent IDs.
 * Search matches the Framework's bounded ASCII-folded metadata profile. */
UmiStatus UmiLearningCatalogueGtk4Search(GtkWidget *widget, const char *query);
UmiStatus UmiLearningCatalogueGtk4Open(GtkWidget *widget, size_t result_index);
/* Resolve an explicit application-owned help route against loaded packs, then
 * open its guide on the existing asynchronous reader path. Map and request are
 * borrowed only until this call returns. Search results need not contain the
 * target and are left unchanged. No pack is loaded or downloaded implicitly.
 * Accepted work owns copied identity and pack metadata; OK is dispatch only. */
UmiStatus UmiLearningCatalogueGtk4OpenHelp(GtkWidget *widget, const UmiLearningHelpMap *map,
    const UmiLearningHelpRequest *request);
UmiStatus UmiLearningCatalogueGtk4Back(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Forward(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Reload(GtkWidget *widget);

/* Follow an explicit local link from the currently displayed resource. Syntax
 * and manifest membership are checked before dispatch; verified bytes are read
 * on the worker. The link is copied before return and uses the restricted
 * learning-navigation profile, not browser URL processing. Search results and
 * last-copy receipts remain unchanged. Failure preserves the reading history.
 * A fragment records a requested anchor; the text preview does not scroll to it.
 * OK means accepted; inspect CopyState for the completed status. */
UmiStatus UmiLearningCatalogueGtk4Follow(GtkWidget *widget, const char *local_link);

/* Open the example declared by the current guide, or reverify the current
 * example. Uses the same per-pack reader/history without copying a file or
 * granting build/run trust. A guide without a declared example is refused. */
UmiStatus UmiLearningCatalogueGtk4OpenExample(GtkWidget *widget);


typedef struct UmiLearningCatalogueCopyReceipt {
    UmiLearningPackInfo pack;
    UmiLearningPackResource selection;
    char destination_root[UMI_PATH_CAPACITY];
    char destination_path[UMI_PATH_CAPACITY];
    UmiLearningPracticeResult result;
} UmiLearningCatalogueCopyReceipt;

/* Resolve the currently displayed resource's example without reading/writing.
 * Refusal leaves out_example unchanged. A guide needs an explicit example link. */
UmiStatus UmiLearningCatalogueGtk4PreviewExample(GtkWidget *widget,
    UmiLearningPackResource *out_example);

/* Accept an explicit copy of that example into a NEW relative file below an
 * existing absolute destination root. Captures immutable pack/selection and
 * copied paths before dispatch. Rechecks source bytes on the worker. No parent
 * directories are created, existing files are never overwritten, and no source
 * is compiled or run. The native adapter limits the source to 1 MiB.
 *
 * OK means accepted, not completed. Closing does not undo an accepted copy:
 * it may finish after retirement and can leave a partial newly created file.
 * Keep the application context running for completion, then inspect the file.
 * The current reader/history is unchanged, including on copy failure. */
UmiStatus UmiLearningCatalogueGtk4CopyExample(GtkWidget *widget,
    const char *destination_root, const char *destination_path);

/* Copy the last completed copy result with its captured identity and paths.
 * NOT_FOUND means no copy result is available; refusal preserves output.
 * A subsequent read/search does not relabel or discard this receipt. */
UmiStatus UmiLearningCatalogueGtk4CopyLastReceipt(GtkWidget *widget,
    UmiLearningCatalogueCopyReceipt *out_receipt);

/* Optional local checkpoint capability. A build without the progress service
 * reports unavailable; it never substitutes temporary in-memory storage.
 * Available reports service composition only; opening storage can still return
 * UNAVAILABLE when the SQLite backend is absent. */
int UmiLearningCatalogueGtk4ProgressAvailable(void);
/* Capture an explicit INSPECT or RECORD request for the displayed GUIDE/LESSON.
 * The request and reader snapshot are independently owned before dispatch.
 * OK means accepted, not stored. Observe busy/last_status, then copy the receipt.
 * No automatic completion, assessment or network operation occurs. The chosen
 * database and receipts are plaintext private learner data.
 *
 * Closing does not cancel or undo an accepted write. Its connection is opened
 * and closed on one worker; orderly application shutdown waits for completion.
 * A result after retirement is discarded. On commit/rollback uncertainty this
 * panel blocks further records until explicit INSPECT succeeds for that same
 * captured database and learner (including a valid no-checkpoint result).
 * Reading another document never changes or relabels the last receipt. */
UmiStatus UmiLearningCatalogueGtk4Progress(GtkWidget *widget,
    const UmiLearningProgressLocalRequest *request);
UmiStatus UmiLearningCatalogueGtk4CopyProgressReceipt(GtkWidget *widget,
    UmiLearningProgressLocalReceipt *out_receipt);


typedef enum UmiLearningCatalogueLibraryAction {
    UMI_LEARNING_CATALOGUE_LIBRARIES_LOAD = 1,
    UMI_LEARNING_CATALOGUE_LIBRARIES_SAVE_NEW = 2,
    UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN = 3
} UmiLearningCatalogueLibraryAction;
typedef struct UmiLearningCatalogueLibraryReceipt {
    UmiLearningCatalogueLibraryAction action;
    char settings_root[UMI_PATH_CAPACITY];
    char settings_path[UMI_TEACHER_PATH_CAPACITY];
    size_t choice_count;
    size_t choice_index;
    UmiLearningLibraryChoice choice;
    UmiRootedFileCreation creation;
    UmiStatus status;
} UmiLearningCatalogueLibraryReceipt;

/* Remember the currently displayed pack's locator, without writing settings.
 * The reader keeps its own admitted revision even after shelf replacement.
 * Forget removes a locator only: shelves, readers and files remain intact.
 * All mutation refuses busy/retired panels; indices belong to the current list. */
UmiStatus UmiLearningCatalogueGtk4RememberLibrary(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4ForgetLibrary(GtkWidget *widget, size_t index);
UmiStatus UmiLearningCatalogueGtk4LibraryChoiceAt(GtkWidget *widget, size_t index,
    UmiLearningLibraryChoice *out_choice);

/* Explicit asynchronous settings operations. Load replaces only the remembered
 * choices after complete decoding; it does not open packs or grant trust.
 * Save creates a NEW private settings file, never overwrites one, and may finish
 * after close. Paths and the saved list are captured before dispatch. Choose an
 * existing private folder outside repositories; no default path is inferred.
 * Reopen reads one remembered manifest and atomically adds its pack/search to
 * the catalogue on success. It leaves the current reading history unchanged.
 * A duplicate shelf ID is refused; use the existing explicit replacement review.
 * OK means accepted. Inspect busy/last_status and the captured receipt later. */
UmiStatus UmiLearningCatalogueGtk4LoadLibraries(GtkWidget *widget, const char *absolute_file);
UmiStatus UmiLearningCatalogueGtk4SaveLibrariesNew(GtkWidget *widget, const char *absolute_file);
UmiStatus UmiLearningCatalogueGtk4ReopenLibrary(GtkWidget *widget, size_t index);
UmiStatus UmiLearningCatalogueGtk4CopyLibraryReceipt(GtkWidget *widget,
    UmiLearningCatalogueLibraryReceipt *out_receipt);


/* Structured reading metadata belongs to the displayed verified snapshot.
 * NOT_FOUND means the current resource uses the retained plain-text fallback.
 * Copy calls do not navigate and preserve outputs on refusal. Links remain
 * untrusted data; OpenReadingLink applies the same local manifest policy as
 * Follow, so schemes/remote URLs never become implicit browser launches. */
UmiStatus UmiLearningCatalogueGtk4CopyReadingSummary(GtkWidget *widget,
    UmiBrowserReadingSummary *out_summary);
UmiStatus UmiLearningCatalogueGtk4ReadingLinkAt(GtkWidget *widget, size_t index,
    UmiBrowserReadingLink *out_link);
UmiStatus UmiLearningCatalogueGtk4OpenReadingLink(GtkWidget *widget, size_t index);
/* Copy inert image-reference metadata from the displayed structured guide.
 * Indices run through CopyReadingSummary().image_descriptions. The source
 * status in a copied entry is independent of this metadata-copy status.
 * OpenReadingImage accepts only a nonempty local reference resolving to a
 * manifest IMAGE without a fragment, then uses the existing verified worker
 * read. Other resources and remote/undeclared paths are refused. No automatic
 * image fetching occurs. OK means admission, not completed PNG rendering. */
UmiStatus UmiLearningCatalogueGtk4ReadingImageAt(GtkWidget *widget, size_t index,
    UmiBrowserReadingImage *out_image);
UmiStatus UmiLearningCatalogueGtk4OpenReadingImage(GtkWidget *widget, size_t index);
/* Position the text cursor at an exact admitted anchor without reading a file
 * or adding history. Missing IDs preserve the current cursor. Mutating reading
 * controls refuse busy/retired panels. Zoom accepts 50..200 percent and affects
 * this panel only; it is neither persisted nor an operating-system preference. */
UmiStatus UmiLearningCatalogueGtk4GoToAnchor(GtkWidget *widget, const char *identifier);
UmiStatus UmiLearningCatalogueGtk4SetReadingZoom(GtkWidget *widget, unsigned int percent);

#define UMI_LEARNING_READING_FIND_CAPACITY 256U
typedef struct UmiLearningCatalogueFindMatch {
    uint64_t reader_revision;
    size_t byte_start, byte_length;
    int wrapped;
} UmiLearningCatalogueFindMatch;
/* Find a literal in the displayed structured guide, using Framework's shared
 * editor search engine. The nonempty query is valid UTF-8, at most 255 bytes.
 * case_sensitive and backwards are 0 or 1. Insensitive matching folds ASCII
 * letters only; other Unicode bytes compare exactly, without normalization.
 *
 * Forward starts after a selection or at the cursor; backward starts before
 * a selection or cursor. Both wrap once. A successful match becomes selected
 * and visible; byte offsets refer to the immutable displayed text, not HTML
 * source. No file is read, history added, document edited or pack searched.
 * NOT_FOUND covers no match, no open guide and the plain-text/image fallback.
 * Refusal preserves selection and output. The optional output is a copied
 * receipt tied to reader revision. Queries are not retained. Mutating calls
 * refuse busy/retired panels; selection observers cannot start another action.
 * Use the catalogue's controls or its Ctrl+F shortcut for interactive search. */
UmiStatus UmiLearningCatalogueGtk4FindText(GtkWidget *widget, const char *query,
    int case_sensitive, int backwards, UmiLearningCatalogueFindMatch *out_match);

/* Metadata copies remain valid independently of widgets. Refusal preserves out.
 * busy remains set during result publication to refuse reentrant operations.
 * Retire before destroying a host that externally retains this widget. It
 * immediately disables new operations; accepted reads finish privately and
 * discard results. Keep the GTK main context running for task cleanup.
 * Repeated Retire is harmless; a retired panel cannot be reopened. */
UmiStatus UmiLearningCatalogueGtk4CopyState(GtkWidget *widget, UmiLearningCatalogueGtkState *out);
void UmiLearningCatalogueGtk4Retire(GtkWidget *widget);

/* Count accepted file jobs (reads or explicit practice copies) whose GTK completion has not finished, across catalogue
 * panels. A host can retire its panels and keep iterating the GTK context until
 * this reaches zero during orderly shutdown. Call on the GTK owner thread. */
size_t UmiLearningCatalogueGtk4PendingReads(void);


/* One explicitly opened manifest IMAGE resource. Static noninterlaced PNG is
 * optional through Framework Media; other formats retain an explanatory text
 * result. Encoded input follows the catalogue's 1 MiB read limit. Decoding is
 * limited to 2048 per dimension and 1,048,576 pixels before allocation.
 * No filename is reopened by the renderer, no remote content is fetched and
 * no SVG/script/animation is executed. Text zoom does not alter image pixels. */
typedef struct UmiLearningCatalogueIllustrationState {
    UmiStatus status;
    size_t width, height;
    int has_pixels;
} UmiLearningCatalogueIllustrationState;
/* Capability query only. A true result does not admit a particular image. */
int UmiLearningCatalogueGtk4PngAvailable(void);
/* NOT_FOUND for a non-image selection. Refusal preserves output. A successful
 * copy may contain a non-OK rendering status while verified bytes and reading
 * history remain available. Allocation failure preserves the prior reader. */
UmiStatus UmiLearningCatalogueGtk4CopyIllustration(GtkWidget *widget,
    UmiLearningCatalogueIllustrationState *out_state);

#ifdef __cplusplus
}
#endif
#endif
