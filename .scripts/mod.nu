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

# Helper to select zig files
def zig-files [] {
  let root = ^git rev-parse --show-toplevel | path expand

  cd $root;
  ls **/*.zig
  | get name
}

# 🬛🬏🬖▌🬜🬊🬓▌🬖🬀🬕🬂🬀
# ▌🬁 ▌▌🬂▌🬕🬋🬏🬴🬰🬏 

# Choose and compile a single Zig file
export def make [
  file?: string@zig-files   # The specific file to compile
  --dry-run(-d)             # Don't compile anything
] {
  # 1. Make sure we are in the git root!
  let root = ^git rev-parse --show-toplevel | path expand
  cd $root;

  # 2. Ascertain which Zig file the user wants to compile
  let zigfile = (
    if $file != null {
      $file
    } else {
      let message = $"(ansi ub)Which file do you want to compile?(ansi reset)"

      ls **/*.zig
      | get name
      | input list --fuzzy $message
    }
  )
  let outfile = $zigfile | path parse | get stem

  # 3. Compile the file and organize the outputs
  if not $dry_run {
    ^zig build-exe $zigfile -O ReleaseFast -fsingle-threaded
    rm -v *.o
    mv -v $outfile bin/
    commandline edit $outfile
  }

  # 4. Optionally add the new binary to .scripts/mod.nu
  # let existing = (
  #   open .scripts/mod.nu
  #   | lines
  #   | where {|x| $x =~ '^\s*export extern' }
  #   | each { str replace -r '^\s*export extern (\S+) \[.*$' '$1' | str trim }
  #   | sort
  # )

  # if $existing not-has $outfile {

  #   sleep 300ms

  #   print $"\nBinary (ansi r)($outfile)(ansi reset) does not have an associated extern."

  #   let new = (
  #     try {
  #       print "Would you like to create one now?"
  #       input "> " --default "yes"
  #       | str downcase
  #       | str trim
  #       | if $in == 'y' { "yes" } else { $in }
  #     } catch { "no" }
  #   )
  #   if $new != "yes" {
  #     print ""
  #     print "You declined to create a new entry in `.scripts/mod.nu`"
  #     return
  #   }

  #   mut stop: bool = false
  #   while not $stop {
  #     print ""
  #     let desc = input "Describe the binary: " | str downcase | str capitalize
  #     print $"> (ansi g)($desc)(ansi reset)"

  #     $stop = (
  #       try {
  #         input "Is this correct? " --default "yes"
  #         | str downcase
  #         | str trim
  #         | if $in == 'y' { "yes" } else { $in }
  #       } catch { "no" }
  #       | if $in == "yes" { true } else { false }
  #     )

  #     if $stop {
  #       let extern = [
  #           ""
  #           $"# ($desc)"
  #           $"export extern ($outfile) []"
  #       ]

  #       open .scripts/mod.nu
  #       | lines
  #       | append $extern
  #       | str join (char newline)
  #       | save --append .scripts/mod.nu;

  #       sleep 300ms

  #       print $"\nAdded extern for ($outfile) to `.scripts/mod.nu`"
  #     }
  #   } # end of loop
  # }
}

# 🬕🬂🬀🬣🬞🬄🬂🬕🬀🬕🬂🬀🬕🬂🬓🬛🬏▌🬳🬂🬃
# 🬴🬰🬏🬖🬈🬏 ▌ 🬴🬰🬏▌🬂🬓▌🬈▌🬢🬰🬃

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
