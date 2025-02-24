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


