# Read an update plan before applying it

Sammy Hegab · Umicom Foundation · MIT

`main.c` is the native CLI entry into Framework's maintenance service.
`review_client.c` is a complete read-only API consumer. It does not replace the
CLI or embed a second installer. Both use `Umicom::setup_centre`.

Begin with `docs/learning/maintain-your-umicom-applications.html`. It explains the
graphical workflow and the difference between an update, exact-byte repair,
selective file retirement, unfinished recovery and completed undo.

To study the C example, first read `review_client.h`. The caller supplies two
borrowed path strings and an output stream. The function opens the installed
catalogue, binds the plan to that receipt, prints the changes and destroys its
owned objects once. Summary and row pointers are borrowed; never free them.

Build this directory with CMake and run CTest. Set
`UMICOM_SETUP_CANONICAL_PROCESS=ON` to include the actual Framework process runner
and its retained tests. The complete example is compiled into the maintenance
regression executable and run by `framework.setup_maintenance.public-example`.
The test makes a new Notes/Stock fixture with inert text payloads. No application
binary is executed. The example proves that a review does not alter the fixture.

Exercises: add a bounded progress display without modifying the plan; explain
why a changed installed-list identity must cause a new review; and explain why
undoing a repair can legitimately restore damaged bytes. Keep actual Apply
behind a separate, explicit user decision. Never treat a digest as a signature.
