# Native implementation policy

Umicom Framework · Sammy Hegab, Umicom Foundation · MIT

## Implementation language and ownership

Start with the lowest practical implementation layer. Portable shared behaviour,
features, utilities, modules and primary development tools belong in C23. Use
Assembly when an instruction-level or architecture-specific implementation is
technically justified. Introduce C++ where the requirement genuinely calls for
it, such as a C++-only vendor interface; preserve a C-compatible public boundary.

Do not choose Python, JavaScript, shell scripts or another higher-level language
merely because it is quicker to write. A proposed exception needs a concrete
technical requirement that cannot reasonably be satisfied by the native layer.
An existing native Framework capability must be checked before adding a parallel
implementation. Reusable behaviour belongs in Framework; product modules remain
thin compositions over its public contracts.

CMake describes the adopted build graph, and HTML/CSS describe public reading
material. They are not a substitute for a native implementation of a product
feature, delivery operation, parser, generator or test runner. Operator commands
shown in PowerShell or a terminal invoke tools; new reusable automation should
be implemented as a native tool rather than hidden in those instructions.

## Preservation during migration

Existing scripts may remain usable as alternative implementations. Do not delete
them, their options, comments, credits or licences merely because a native
replacement is now preferred. Do not invoke a retained script silently when the
native operation fails; report the failure or unsupported capability explicitly.

For superseded C or C++ implementations that must no longer execute, retain the
previous implementation in an explicit disabled block, normally `#if 0`. Put an
engineering comment immediately above it explaining the reason for replacement,
the active replacement and why the old source is retained for review. Explain
the architectural reason for the active replacement as well. The project owner
makes the final decision about physical deletion.

Preserve public API names and layouts, file headers, authorship, licence notices,
educational comments and important implementation comments. A migration must not
silently remove behaviour under the description of cleanup or simplification.
Where a new input restriction is necessary, document it and test the rejection.

## Testing and delivery

Use native C tests for new C capabilities, including invalid inputs, partial
failure and ownership/lifetime boundaries. Keep alternative implementations
available for explicit comparison. State which operating systems, compilers,
interfaces and end-to-end workflows actually ran; a fixture is not a production
application, and an unsupported operation is not a passed test.

Deliver full repository-relative files. Archive roots mirror the actual local
checkouts: `Umicom-Applications` and `umicomOS`. Do not rename either directory,
introduce a wrapper folder, overwrite whole destination trees or omit local-only
files during a merge. Include build, test and changed-repository publication
instructions. Commit directly to `main`, publish child repositories first, and
record parent pins last. Never discard local work to make a checkout look clean.

Public guides teach an understandable, real workflow step by step and use the
Umicom logo and icon. Validation logs, manifests and engineering evidence are
separate from the public lesson.

## Current native migration checkpoint

`umicom-session-stage` is the primary implementation of Linux launcher planning,
staging, verification and local-file digests. Its library and primary tests are
C23. It does not depend on an interpreter, OpenSSL or a third-party JSON library.
The existing `scripts/stage-desktop-session.py` and
`umicomOS/tools/desktop_session.py` remain unchanged alternatives.

This checkpoint does not claim that every earlier script has been migrated.
The OS-image builder and other existing script-based workflows remain separate
migration work. Replacing a wrapper while delegating its work back to Python
would not satisfy this policy.
