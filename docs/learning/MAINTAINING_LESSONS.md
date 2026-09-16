# Maintaining the lessons

## One home for teaching content

Keep the shared catalogue in `src/teacher/foundations_curriculum.c`, reading material in `docs/learning`, and runnable examples in `examples/learning`. Applications consume the catalogue. An example may demonstrate a small idea, but it must not become an alternative implementation of a production Framework service.

Existing lesson identifiers are stable. The older `foundations.branch` and `foundations.pull-request` identifiers remain for stored progress and callers. Their visible titles and exercises now teach working on main and publishing a reviewed change. No public structures or functions were renamed.

## A lesson that can be followed

Name the task directly. Introduce each tool before asking the reader to use it. State which terminal to open, where the files belong, the exact command, the expected result and how to interpret a failure. Keep Windows and Linux instructions separate. Use small exercises, questions with explanations and optional deeper sections. Do not label the reader by age, experience or ability.

Use original Umicom artwork. Preserve real author, licence and provenance information. Do not personalise public instructions or mention a conversation. Do not describe a reading page as a finished application feature.

## Code and tests

Use C23 and Assembly for Umicom-owned executable work. CMake describes the build; HTML describes the lessons. The examples use no new runtime scripting language. Keep architecture-specific Assembly isolated and state both the instruction set and ABI. Never claim a foreign object file was executed just because it assembled.

Compile console examples as separate small targets. Add boundary and failure cases where they demonstrate the contract. Keep checks active in Release builds. GUI examples must call actual supported Framework interfaces and distinguish compilation, execution and interaction evidence.

Run the catalogue test and the resource test after changing a lesson. The resource test rejects a missing page, wrong or duplicate lesson identity, mismatched title or heading, incomplete document envelope, duplicate local anchors and broken same-directory navigation. It reads actual tags rather than accepting an identity hidden in a comment or script. Preserve the original C reference and other existing documentation. Check all links in the published reading copy.

## What remains to develop

| Outcome | Next implementation work | Evidence required |
|---|---|---|
| Lessons from any workspace | Resolve source and installed learning resources through the existing Framework resource services | Start Studio outside the Applications checkout and open every lesson |
| Installed SDK first-project workflow | Qualify the GTK-enabled exported SDK and its resource discovery | Build and run the separate window project on Windows and Linux |
| Learning progress in the application | Connect the existing teacher progress and assessment contracts to recorded exercise results | Tests for save, reopen, failure and corrected submission; no duplicate progress store |
| A growing C sequence | Add structs, file operations, allocation, ownership and error-handling exercises | Executable examples, boundary tests and line-by-line explanations |
| A growing GUI sequence | Add commands, model/view separation, layout editing and persistence using current Framework APIs | Real click, resize, close, restart and recovery tests |
| Further architecture lessons | Add ARM64 and RISC-V runtime exercises without copying platform services | Native or explicitly emulated execution evidence per ABI |

The supplied progress file is a manual record. Browser checklists do not update code, run tests or certify mastery. Keep private learner data outside the public repository.

## Canonical naming and compatibility

Use `git-first-commit.html` for the Git lesson. `git-first-comment.html` remains
only as a small bookmark-compatibility page. Do not add that compatibility page
to the lesson catalogue or give it a new progress identifier. The 20 catalogue
IDs, order and exercises remain unchanged by the naming correction.

`naming-and-ownership.html` is an additional reference for contributors, not a
21st assessed lesson. The course index and architecture/change lessons link to
it without changing any learner's sequence or progress.

The resource test intentionally supports our authored lesson subset: quoted
attributes, a plain-text title and h1 (with amp/lt/gt/quot/apos named entities),
and filename-only local navigation with optional fragment IDs. It checks the 20 catalogue pages plus the course index, workshop guide, naming
reference and compatibility page. It checks anchor and canonical links, not all
image/CSS/script assets. HTTP, HTTPS and mailto links
are classified as external and never fetched. Unsupported local URL forms,
malformed inputs, oversized documents and embedded zero bytes fail qualification;
they are not silently skipped. Resource parsing is a test, not a browser or a
security sandbox. Keep the checkout trusted and do not use it to scan secrets.

Run `framework.teacher.foundations.resources` and its `markup`, `links` and
`bounds` checks. The same tests can be built without the full GUI dependency
graph using `tests/release_qualification`. The independent naming, catalogue
and resource checks keep their assertions and setup active in Release builds.
A passing static resource check does not establish that Studio can resolve an
installed resource from an unrelated working directory; that runtime work stays
in the outstanding table above.
