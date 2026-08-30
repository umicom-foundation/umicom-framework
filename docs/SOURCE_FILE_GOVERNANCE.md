# Source File Comments and Declaration Safety

Every C implementation file and reusable `.inc` fragment begins with a short
Umicom file comment. The comment helps a new developer answer four questions
before reading the code:

1. Which project owns this file?
2. Where does it live in that project?
3. What job does it perform?
4. Who owns the work and which licence applies?

The required shape is:

```c
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/example/example.c
 *
 * PURPOSE:
 *   Explain the real job of this file in ordinary language.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
```

Write the purpose for a person, not for a tool. “Utilities” or “miscellaneous
code” is too vague. A useful purpose says what the file implements, verifies or
declares. Keep detailed design explanations near the relevant function or in a
guide; the file comment is a signpost.

## Automated checks

`UmicomSourceCommentAudit.cmake` checks every C and `.inc` implementation file
in Framework and the application portfolio. It skips generated build folders
and checks for the file, purpose, author, organisation and licence fields.

`UmicomDeclarationDependencyAudit.cmake` protects catalogue call sites. A file
that calls a recipe or experience catalogue must include the exact public
contract, or a documented aggregate header that includes it. This catches the
kind of problem where C guesses that an undeclared function returns `int`, then
reports a second pointer-conversion error.

The rule is simple: include the header that declares the function you call.
Do not depend on an unrelated header happening to include it today.

## Adding a new source file

- Copy the comment template and change the project, path and purpose.
- Include the new file's own public or internal header first when practical.
- Add direct headers for every public service family the file calls.
- Register the source and its focused test in CMake.
- Keep generated catalogue fragments readable; `.inc` files follow the same
  ownership and purpose rules as `.c` files.

These checks do not replace compilation. They catch predictable maintenance
mistakes early, while the compiler and test suite still verify types and
behavior after the developer builds the project.
