Created by: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT
# Appearance profiles

The Umicom Framework appearance model is a toolkit-neutral catalogue of
semantic colours, typography, density and accessibility preferences. Product
frontends render the active profile without putting GTK, Qt or web types in the
public model.

## Responsibilities

- Framework owns profile validation, activation, cloning, protection and the
  versioned persistence codec.
- Each application owns its branding assets and chooses the built-in profiles
  it presents.
- Frontend adapters read the active snapshot and map semantic tokens to native
  controls.

Built-in profiles are protected from replacement or removal. A user or product
extension can clone a built-in, edit the clone, lock it and persist it through
`umi_ui_appearance_profile_encode`. Exactly one profile can be active at a
time.

Brand artwork is referenced by a runtime resource path; image bytes and private
developer paths are never stored in the Framework model. This lets installed
applications place the official artwork next to their executable while future
frontends use the same appearance data.

## Accessibility and portability

Profiles expose comfortable, compact and spacious density modes, bounded font
scaling and a reduced-motion preference. All colours use semantic names such as
`surface`, `foreground`, `accent`, `warning` and `danger`. The high-contrast
profile is suitable for a frontend that needs strong focus separation and
reduced decorative motion.

The persistence record starts with `appearance-v1`. Decoders validate every
field before publishing a profile, allowing the format to evolve without
silently accepting malformed settings.
