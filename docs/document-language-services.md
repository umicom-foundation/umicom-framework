<!--
Umicom Bits2Banking
File: docs/document-language-services.md

PURPOSE:
Use shared document identities, language choices and bounded file filters

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->
# Document language services

Use Framework's document services to identify text files consistently across an editor, a command-line tool and a file picker. A language ID describes an editing policy. It does not prove that a compiler, debugger, simulator or syntax-description package is installed.

## Begin with a name

1. Include umicom/document/languages.h.
2. Create a UmiDocumentLanguageIdentity value.
3. Pass a readable, terminated filename and the result's address to umi_document_language_detect.
4. Check the returned UmiStatus before using the result.
5. Use UmiDocumentLanguageChoiceFind to obtain a display description, if one is registered.

The detector examines a bounded basename. Directory separators are recognised in both Windows and Unix form. Exact suffix matches precede case-insensitive fallback so .C retains its C++ meaning. Special basenames include CMakeLists.txt and Makefile. Unknown names resolve to plain text; no file contents are read.

## Ownership

UmiDocumentLanguageChoiceCount and UmiDocumentLanguageChoiceAt expose an immutable static catalogue. A successful Find returns a borrowed entry. Do not free or modify it. ChoiceIdentity derives an identity from the catalogue's example filename and verifies that the result has the selected ID; failure preserves the caller's output.

Language choices include VHDL, Verilog, SystemVerilog, Zig, D, Dart, Pascal and PHP, alongside existing languages. Clients share these descriptions rather than copying the catalogue into each application.

## File filters

Include umicom/document/file_filter.h and initialise a set with umi_document_file_filters_default. Keep large sets in appropriately owned storage rather than placing many copies on a small thread stack.

The original first five categories retain their positions. Additional groups are Programming Languages, Hardware Description Languages and Build Files. All Files remains available for unfamiliar formats.

The matching helper accepts all-files patterns, suffix patterns such as *.vhd, and literal basenames such as CMakeLists.txt. It compares ASCII case-insensitively and rejects empty names or names ending in directory separators. It is not a full shell glob engine. A GUI toolkit may have different case-matching rules and needs platform-specific checks.

Every stored pattern must terminate inside its capacity, and pattern_count must remain within the array bound. The matcher validates the whole stored set before matching. Adding a pattern rejects overlong input or a full array before changing count; memmove permits input that aliases an existing pattern. The caller still must provide valid readable storage. These checks cannot validate an arbitrary pointer.

## Editing and toolchains

UmiEditorLineCommentStyleFind provides a separate immutable profile. Unsupported profiles return NOT_FOUND. Pascal has no shared line-comment profile here; callers must not guess a dialect. GtkSourceView syntax descriptions are another dependency. An absent grammar can leave a recognised language displayed as plain text.

Project compilation belongs to the project build service. A VHDL workflow needs a simulator or synthesis tool explicitly configured for that project; it is not compiled by the C toolchain that builds the editor.

## Regression boundaries

The document-language suite covers identity, choice consistency and default filters. The boundary suite covers malformed counts, unterminated storage, maximum input length, full capacity, overlapping input, basenames, literals, wildcard rules and invalid arguments.

Keep native UI coverage separate: Open dispatch, chooser visibility, cancellation, accepted paths, document activation, editing and tab changes each need observable outcomes. Passing catalogue tests does not establish that a user's frozen window is repaired.

## Teaching example

The Bits2Banking lab labs/native/framework-language-inspection links the installed document and editor SDK components. Its source and Learning Umicom Framework module explain these calls step by step. The lab examines names only and needs no window or broker session.
