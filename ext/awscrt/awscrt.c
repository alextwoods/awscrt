#include "awscrt.h"

#include <aws/common/clock.h>
#include <aws/common/allocator.h>
#include <aws/common/cbor.h>


VALUE rb_mAwscrt;

static VALUE my_clock(VALUE klass) {
    uint64_t output;
    aws_sys_clock_get_ticks(&output);
    return ULL2NUM(output);
}

static VALUE test_cbor(VALUE klass) {
    struct aws_cbor_encoder *encoder = aws_cbor_encoder_new(aws_default_allocator());

    aws_cbor_encoder_destroy(encoder);
    return UINT2NUM(1);
}

void cbor_encoder_free(void* data) {
  struct aws_cbor_encoder *encoder = (struct aws_cbor_encoder*) data;
  aws_cbor_encoder_destroy(encoder);
}

static const rb_data_type_t cbor_encoder_type = {
	.wrap_struct_name = "cbor_encoder",
	.function = {
		.dmark = NULL,
		.dfree = cbor_encoder_free,
		.dsize = NULL,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE cbor_encoder_alloc(VALUE self)
{
  struct aws_cbor_encoder *encoder = aws_cbor_encoder_new(aws_default_allocator());
	return TypedData_Wrap_Struct(self, &cbor_encoder_type, encoder);
}

VALUE cbor_encoder_reset(VALUE self) {
  struct aws_cbor_encoder *encoder;
  /* unwrap */
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);
  aws_cbor_encoder_reset(encoder);
  return Qnil;
}



RUBY_FUNC_EXPORTED void
Init_awscrt(void)
{
  rb_mAwscrt = rb_define_module("Awscrt");
  rb_define_singleton_method(rb_mAwscrt, "my_clock", my_clock, 0);
  rb_define_singleton_method(rb_mAwscrt, "test_cbor", test_cbor, 0);

  VALUE cborEncoderClass = rb_define_class_under(rb_mAwscrt, "CborEncoder", rb_cObject);
  rb_define_alloc_func(cborEncoderClass, cbor_encoder_alloc);
  rb_define_method(cborEncoderClass, "reset", cbor_encoder_reset, 0);

}
