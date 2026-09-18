# Public website and Wiki sources

This directory builds a public documentation website from Framework and the
application-owned introductions. It complements the existing `umicom-documentation`
target and uses the same Doxygen configuration for the optional code reference.
There is no second C/C++ parser and no new application service in this directory.

From a full Applications checkout:

```text
cmake -S framework/docs/site -B build/documentation -G Ninja
cmake --build build/documentation --target umicom-website
ctest --test-dir build/documentation --no-tests=error --output-on-failure
```

Open `build/documentation/site/index.html`. With Doxygen installed, reconfigure
and build `umicom-website-api` to add the function/type/source browser. Build
`umicom-website-package` for a distributable offline site ZIP. The generated
`publishing.html` explains Windows, Linux and publication boundaries.

## Content ownership

- Framework owns reusable teaching topics, templates and the shared presentation.
- Product introductions live in `applications/<product>/docs/site/index.html`.
- Existing application guides are copied into the output, not duplicated in source.
- Example blocks are read from their existing C files at configure time.
- The public header inventory is generated into the build tree.
- Download pages link to real repository release listings; no guessed asset exists.
- Generated sites, logs and archives do not belong in source control.

The initial worked topics cover documents, compiler diagnostics and component
composition. Four detailed product introductions cover Studio, Trader, Desk and
Bank. The catalogue links all 24 application repositories; the other application
user guides remain planned, not automatically complete because a link exists.

## Contribute a topic

Explain a real task, list its exact public header, implementation, tests and build
target, and include a complete runnable example. Document ownership, parameters,
return values, thread rules, limits and failure handling beside the code.
A public function index is not a complete tutorial for every listed symbol.
Use `docs/WRITING_DOCUMENTATION.md` for the general writing guidance.

Keep stable filenames, application names, APIs and source directories. Extending
coverage must not cause unrelated renaming or source rewrites. Do not copy private
reports or whole internal documentation directories into public web output.

## Hosting

This builder does not deploy, configure domains or modify the existing Foundation
website. Review a hosting destination and publication workflow separately. Use a
static-site host for the output. Link any GitHub Wiki to the same canonical content
or publish a one-way generated mirror; do not create another independently edited
API reference. Future installed Help should consume a version-matched copy of the
same public documentation through Framework's existing resource services.
