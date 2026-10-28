# C++ style

- Keep implementations minimal and direct.
- Use blank lines to separate loosely related steps and give larger blocks space.
  Keep closely related statements grouped together.
- For functions and constructors with a one-line body, omit blank lines between
  the body and its surrounding braces.
- Prefer shorter declarations before longer ones when the ordering is natural.
  Dependencies and logical grouping take priority; do not force length ordering.
- Use `sluggish/compiler/parse/Parser.cpp` as a reference for the general spacing
  style, with the one-line-body exception above.
