# Uli's magic auto-loading scripts
#
# Bitform Font

# export use some-module.nu *

# Add the binaries to the PATH
export-env {
  let bin = ^git rev-parse --show-toplevel | path expand | path join bin
  $env.PATH = $env.PATH | append $bin
}

# Get the exported scope from this module
export def main [] {
  scope modules
  | where name == 'scripts'
  | get 0
}

# Print a single glyph character from an int array.
export extern bt-array-to-unicode-sextant []

# The AI slop C version of this app.
export extern bt-chatgpt-helpful-slop []

# For the numbers 0-200, emit an ascii value
# if it matches a valid glyph.
export extern bt-emit-valid-ascii-in-200 []

# Print the valid glyph ascii values, treating
# the characters as ints.
export extern bt-for-loop-with-ascii-ints []
