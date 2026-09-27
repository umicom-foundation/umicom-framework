# Native designer project contract

Umicom Framework · Sammy Hegab, Umicom Foundation · MIT

## Authority and lifetime

The input is the existing `UmiDeclDocument`, not a second saved design format. Native validation copies canonical nodes into temporary traversal storage. Creation returns an immutable `UmiDesignerNativeProject` whose generated text no longer depends on that document. Mutations and creation use the document owner's thread. Concurrent reads of a plan require an externally retained lifetime; destroy must not race a reader or publisher. A `UmiDesignerNativeFileView` borrows text until plan destruction.

`UmiDesignerNativeNotesDocument` constructs an ordinary canonical seven-node model. `UmiDesignerNativeProjectCreate` validates before returning a plan. `UmiDesignerNativeProjectPublish` is the explicit filesystem operation; neither validation nor plan creation touches disk. Null result handles are published on failed creation. Public result structures remain caller-owned. Existing public APIs, layouts, serializers and source-generation interfaces are retained.

## Supported native profile

Version 1.0.0; 1–128 nodes, depth at most 32; exactly one visible root window and one content child. Components: window, pane, split, tabs, label, text, editor, button. Pane orientation is horizontal or vertical and spacing is 0–64. A split has exactly two children; tabs have 1–16 children. Leaves have none. Unknown types/properties are rejected, not dropped. Common properties are title, tooltip, visible and enabled; titles can label tab pages. Window width/height are 240–8192; split position 0–8192. Strings follow existing 511-byte canonical fields. Names and target references stay canonical. Native caption strings must contain valid UTF-8 scalar values. Fields with contradictory textual and typed values are rejected.

The only executable UI binding is `text.count` from a text/editor source to a label target. No binding invokes a shell, network, database, file save, arbitrary code or another application. Unbound buttons are disabled. GTK drafts are temporary; entry limit 4096 scalar values, editor limit 262144. The count is scalar-based, not grapheme-based. GTK owns widgets; the API returns a separately referenced window. Call `gtk_window_destroy`, then `g_object_unref`. `UmiDesignerNativeGtkRefControl` returns an owned reference, and rejects detached controls. Callbacks use weak owner/control references and check the originating root.

## Source and publication

Project names match `[a-z][a-z0-9_]{0,62}`. Generated filenames are six fixed constants. Values are emitted as quoted C byte strings using exact three-digit octal escapes where necessary; no model value is executable CMake syntax. The generated model uses the same canonical API as the designer. GUI dependencies are explicit: requesting GTK from a headless SDK fails configuration. `--check` is model validation, not graphical acceptance.

Publishing requires a new absolute directory with an existing parent. Existing files, directories and links are refused; no caller data is deleted. POSIX uses a held directory fd and exclusive relative file creation, file fsync and final directory fsync. Windows uses UTF-16 paths, CREATE_NEW files, file flushing and a held non-reparse directory handle without delete sharing. The directory path must be user-controlled and trusted: ancestor substitution by a hostile actor is outside this API's guarantee. Windows drive-absolute paths are supported by the implementation; UNC/device paths and reserved DOS names are refused. Windows compilation, Unicode argv behaviour and execution still require qualification.

On error, completed-file counts and directory-created status describe the partial result. Incomplete directories remain for review. The last marker lists source revision, node count and lengths; it is not cryptographic verification, a successful compiler/test result or proof against power loss. Multi-file export is not a filesystem transaction. If final directory flushing fails after the marker was written, the returned `complete` flag remains false.

## Compatibility fixes

Subtree removal now discovers every descendant before compaction, preserves unrelated order and advances one revision. Original source remains in an explained disabled block. Node operations validate fixed arrays and construct replacements before mutation. Bad counts, malformed strings and rejected property input no longer damage the old node. The lexer rejects a missing closing quote; its other grammar rules remain unchanged.

The older `.umiapp` parser still infers types even inside quotes. Direct C construction preserves explicit types; text import does not introduce a new typed-string/escape grammar. Existing `umi_decl_serialize_file` and `umi_designer_source_generation_generate` are unchanged alternatives. This batch does not make their output equivalent to the new native profile, retrofit their filesystem policy, or claim to complete authoring Undo/Redo.

## Build composition

`Umicom::designer_native` links the existing `Umicom::declarative`. GTK is a separate `Umicom::designer_native_gtk4` target. Full composition is deferred until the existing targets exist; tests are included using explicit source/binary roots because CMake disallows `add_subdirectory` during deferred execution. A minimal Desktop System composition without the canonical declarative target is not automatically expanded.

The focused SDK under `examples/designer_native` compiles canonical production dependency files. It exports `UmicomDesignerNativeConfig.cmake`, not a replacement full Framework package. The OS wrapper only composes host tools. There is no kernel, filesystem image, installer or guest startup change.

## Validation boundary

The included native regressions and generated-build workflow use actual files, actual compilers and the canonical model. Allocation/I/O wrappers are test-only. Six GTK lifecycle cases are included, but must not be reported as passes until compiled and run on a real GTK build. The full Framework/Applications build and installed Windows startup remain required local acceptance checks.
