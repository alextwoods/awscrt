#include "awscrt.h"

VALUE rb_mAwscrt;

RUBY_FUNC_EXPORTED void
Init_awscrt(void)
{
  rb_mAwscrt = rb_define_module("Awscrt");
}
