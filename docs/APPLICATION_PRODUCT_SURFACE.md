# Building an Application from Framework Pieces

Umicom Framework now provides a shared product surface. A product surface is
the small piece of code that turns an application recipe into a running user
interface model. It does not draw GTK, Qt or web widgets itself. It prepares the
same panels, states and events for any frontend.

Think of a recipe as the instruction sheet in a box of building bricks:

- components are the reusable bricks, such as an editor, chart or payment panel;
- a recipe says which bricks are used and where they are placed;
- a window specification describes the outer window;
- a runtime policy says when panels refresh, sleep and save checkpoints;
- a product controller supplies honest product-specific messages and commands;
- a frontend host turns the finished model into GTK, Qt, web or test controls.

An application should not copy the lifecycle that connects these pieces. The
`UmiApplicationPresentationProductSurface` owns that common work.

## Three layouts for different people

Every supported application can recommend one of three recipe audiences:

- **Learning** shows fewer panels and gives a new user a calm place to start.
- **Standard** shows the balanced everyday workspace.
- **Focus** removes distractions for one concentrated task.

Call `umi_application_presentation_product_surface_init_for_audience()` with
the application ID and audience. Framework finds the matching recipe. It also
checks that the recipe belongs to that application before anything starts.
This prevents, for example, a Bank process from accidentally opening a Studio
recipe.

## What happens during startup

The product surface performs these steps in order:

1. Find and project the requested recipe.
2. Check that the application ID and recipe belong together.
3. Create the toolkit-neutral runtime state.
4. Bind a deterministic headless host.
5. Ask the product to register any small controllers it needs.
6. Mount every visible panel and focus the recipe's starting panel.

If a step fails, startup stops and cleans up the partial surface. The caller
receives the real `UmiStatus`; the application must not pretend startup worked.

## Adding a new application

Start with Framework-owned data, then add a thin product layer:

1. Add reusable component contracts to Framework only when another product
   could reasonably use them.
2. Add learning, standard and focus recipes to the Framework catalogue.
3. Add the matching panel specifications, window specifications, behavior
   profiles and runtime policies.
4. In the application repository, create one opaque application-surface type
   containing `UmiApplicationPresentationProductSurface`.
5. Keep the application's existing public API, but forward lifecycle calls to
   the product surface.
6. Register one product controller for every selected panel with
   `umi_application_presentation_surface_runtime_register_controller_for_all()`.
7. Test all three audiences without depending on a graphical display or live
   external service.

Product controllers should be small. They may describe an empty state, convert
a refresh into product data, or validate a command. Financial applications
must never report a command as executed unless the authorized service really
accepted it. A safe controller can stage the command and return a
`PERMISSION_REQUIRED` presentation state.

## Connecting a graphical frontend

The built-in headless host makes startup deterministic and testable. A GTK, Qt
or web frontend can later bind its own `UmiApplicationPresentationSurfaceHost`
operations to mount, update, focus and unmount real controls. Product logic
still talks to the same surface runtime, so changing the toolkit does not
change the application contract.

## Rules that keep the model reusable

- Put reusable components, recipes, layouts, state and policy in Framework.
- Put product wording and product-only orchestration in the application.
- Do not put GTK widgets inside a Framework component contract.
- Do not duplicate a component just to change where it appears; create a new
  recipe instead.
- Do not hard-code a list of standard-layout panels in a controller registrar.
  Register against the panels in the selected recipe so learning and focus
  layouts continue to work.
- Keep old public product functions when migrating. They can delegate to the
  shared surface, which lets existing frontends continue to compile.

This division is what makes a future Umicom application feel like assembling
building bricks rather than starting a new program from nothing.
