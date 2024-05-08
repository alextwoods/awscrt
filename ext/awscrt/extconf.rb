# frozen_string_literal: true

require "mkmf"

# Makes all symbols private by default to avoid unintended conflict
# with other gems. To explicitly export symbols you can use RUBY_FUNC_EXPORTED
# selectively, or entirely remove this flag.
append_cflags("-fvisibility=hidden")

$INCFLAGS << " -I#{File.join(__dir__,  '../../crt/install/include')}"


# $LIBPATH << "/Users/alexwoo/.gem/ruby/3.0.0/gems/aws-crt-0.2.0-x86_64-darwin/bin/x86_64/"
# $libs << "-laws-crt-ffi"
$LIBPATH << File.join(__dir__, "../../crt/install/lib")
$libs << "-laws-c-common"

create_makefile("awscrt/awscrt")
