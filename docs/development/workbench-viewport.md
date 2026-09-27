# Shared workbench viewport

Author: Sammy Hegab, Umicom Foundation. Licence: MIT.

## Scope and authority

`Umicom::workbench_viewport` consumes the existing canonical
`UmiWorkbenchLayoutDocument` from `workbench_layout/document.h`. It does not
introduce another layout schema, serializer, registry, Data Server or saved-state
format. The existing document APIs, flat desktop projection, layout browser and
application workspace implementations are unchanged.

The core derives rectangles for a single window/content tree. It returns an
explicit error for nested top-level windows rather than flattening them silently.
Binary splits, tab groups, panel/editor leaves and empty leaves are supported.
`active_child_index` is a position in a container's child list. AUTO visibility
currently means visible; preferred_size and monitor fields are not placement rules
in this projection. Unknown flags are retained in the source but are not grants
of authority. Rendering has no business side effects.

## Bounds and arithmetic

Canonical limits remain 256 nodes and 16 child references per node. This adapter
additionally limits reachable depth to 64. The entire graph is validated, including
hidden/inactive branches: indices, unique IDs, parent reciprocity, unique incoming
edges, one root and full reachability. Used node strings must terminate within
their actual array and contain valid UTF-8 without ASCII controls. Document
metadata not used by the projection is not interpreted or copied into a widget.

All rectangles use logical pixels and checked int64_t intermediate sums. Split
ratios are explicitly finite and in [0.05,0.95]. Minima are propagated bottom-up.
A feasible split clamps its requested ratio to both minima; insufficient space is
reported in the output instead of creating negative/out-of-parent rectangles.
Hidden branches consume no split gap. The plan is published only after successful
validation and derivation. Output storage is cleared on failure.

## Lifetime and concurrency

The core performs no heap allocation, no I/O and no callbacks. Its input is
borrowed and must remain stable for the duration of the call. Inputs, output and
diagnostic storage must not overlap. Independent calls with independent outputs
may run concurrently. The plan is a derived snapshot, not permission to mutate
an application.

The GTK adapter is GTK-thread-only. It owns two document copies and references
to all parented children. It constructs leaves once, including initially hidden
ones, and changes their allocation/visibility for preview edits. Factories transfer
one unparented widget; they may return a floating reference or one owned strong
reference. Factory context is borrowed synchronously. Null means failure; an
invalid pointer or borrowed/parented reference violates the factory contract.

Each tab closure holds a GWeakRef to the viewport. Retaining a tab cannot retain
its owner or access the disposed viewport. Disposal unparents children, clears
borrowed child arrays and prevents further mutations. GskTransform ownership is
transferred to gtk_widget_allocate. Gallery signal connections use object-bound
closures; replacing a notebook filter destroys its previous owned query string.

The caller must wrap a viewport in a GtkScrolledWindow. Actual native child
measurements raise adapter-local minima without changing the canonical document.
The adapter clips each child's snapshot to its planned rectangle. F6 navigation
uses the derived visible-leaf order; native Tab behaviour stays with GTK controls.
Layout changes emit `layout-changed` with no arguments and affect only the owned
preview. They do not increment canonical revisions, refresh its content hash,
persist, invoke commands or return a changed canonical document.

## Targets and integration

The full build appends core registration to UmicomDesktopSystem.cmake and
appends the GTK library/gallery to UmicomDesktopWorkspaceGtk4.cmake. Previous
Desk controls are not changed. The learning executable is
`umicom-component-workbench`; the headless lesson is `umicom-layout-example`.
No application-module source or submodule commit is required.

The focused project optionally builds actual GTK4 through pkg-config. Its core
SDK exports only the implemented viewport target and required canonical headers.
It does not pretend to export a complete Framework or GUI SDK. The separate OS
host entry composes the same source; it does not create or qualify an OS image.

## SQLite target compatibility

Call `umicom_resolve_sqlite_target` after find_package(SQLite3). It returns
SQLite3::SQLite3 when available; otherwise it creates that alias to the visible
legacy target. The GLOBAL include guard only guards the function definition.
Resolution occurs on each call in its calling directory to support local imported
targets in sibling projects. Neither target available is a configuration error.

This batch migrates the Data Safety test and standalone SDK paths. It does not
sweep every older SQLite reference in the entire repository or suppress developer
warnings. Existing original link statements are retained in explained inactive
CMake blocks.

## Verification boundaries

Native projection and Data Safety tests use real canonical headers/Data Server
source. CMake target fixtures model old/new/deprecated target metadata; the test
host runs CMake 3.31.6, not the user's newer CMake. The available real SQLite
consumer and installed-SDK consumer are separately built. GTK tests require the
real library/display and are not counted as headless passes.

GTK and Windows paths were not compiled/run in this delivery environment.
High-DPI, screen-reader, focus, theme and installed launch qualification remain
required. No blanket memory-leak or application-completion claim follows from
the native tests.

## Primary references

- CMake FindSQLite3: https://cmake.org/cmake/help/latest/module/FindSQLite3.html
- GTK allocation/transform ownership: https://docs.gtk.org/gtk4/method.Widget.allocate.html
- GTK custom parent ownership: https://docs.gtk.org/gtk4/method.Widget.set_parent.html
- Canonical layout headers at Framework 54563cfe9a920fbd22ce84b2668d510903a99518.
