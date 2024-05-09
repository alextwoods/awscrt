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
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);
  aws_cbor_encoder_reset(encoder);
  return Qnil;
}

VALUE cbor_encoder_get_encoded_data(VALUE self) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);
  struct aws_byte_cursor out = aws_cbor_encoder_get_encoded_data(encoder);
  return rb_str_new((const char *)out.ptr, out.len);
}

VALUE cbor_encoder_add_string(VALUE self, VALUE value) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  struct aws_byte_cursor from;
  from.ptr = (uint8_t *)StringValuePtr(value);
  from.len = RSTRING_LEN(value);
  aws_cbor_encoder_write_text(encoder, from);
  return Qnil;
}

VALUE cbor_encoder_add_pos_int(VALUE self, VALUE value) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  aws_cbor_encoder_write_uint(encoder, NUM2ULONG(value));
  return Qnil;
}

VALUE cbor_encoder_add_neg_int(VALUE self, VALUE value) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  aws_cbor_encoder_write_negint(encoder, NUM2ULONG(value));
  return Qnil;
}

VALUE cbor_encoder_add_float(VALUE self, VALUE value) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  aws_cbor_encoder_write_float(encoder, RFLOAT_VALUE(value));
  return Qnil;
}

VALUE cbor_encoder_start_array(VALUE self, VALUE size) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  size_t s = NUM2UINT(size);
  aws_cbor_encoder_write_array_start(encoder, s);
  return Qnil;
}

VALUE cbor_encoder_start_map(VALUE self, VALUE size) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  size_t s = NUM2UINT(size);
  aws_cbor_encoder_write_map_start(encoder, s);
  return Qnil;
}

// used below to encode hash entries.  Defined below
int encode_hash_entry_iter_func(VALUE key, VALUE value, VALUE self);

// https://docs.ruby-lang.org/en/master/extension_rdoc.html
// cbor gem similiar code: https://github.com/cabo/cbor-ruby/blob/master/ext/cbor/packer.c#L132
VALUE cbor_encoder_crt_auto_add(VALUE self, VALUE value) {
  struct aws_cbor_encoder *encoder;
  TypedData_Get_Struct(self, struct aws_cbor_encoder, &cbor_encoder_type, encoder);

  long v;
  struct aws_byte_cursor from;

  switch (TYPE(value)) {
    case T_FIXNUM:
      v = FIX2LONG(value);
      if (v < 0) {
        aws_cbor_encoder_write_negint(encoder, (uint64_t)(-1-v));
      } else {
        aws_cbor_encoder_write_uint(encoder, (uint64_t)(v));
      }
      break;
    case T_FLOAT:
      aws_cbor_encoder_write_float(encoder, RFLOAT_VALUE(value));
      break;
    case T_STRING:
        from.ptr = (uint8_t *)StringValuePtr(value);
        from.len = RSTRING_LEN(value);
        aws_cbor_encoder_write_text(encoder, from);
      break;
    case T_SYMBOL:
        value = rb_obj_as_string(value);
        from.ptr = (uint8_t *)StringValuePtr(value);
        from.len = RSTRING_LEN(value);
        aws_cbor_encoder_write_text(encoder, from);
      break;
    case T_ARRAY:
      v = RARRAY_LEN(value);
      aws_cbor_encoder_write_array_start(encoder, v);
      for(long i = 0; i < v; i++) {
        cbor_encoder_crt_auto_add(self, rb_ary_entry(value, i));
      }
      break;
    case T_HASH:
      v = RHASH_SIZE(value);
      aws_cbor_encoder_write_map_start(encoder, v);
      rb_hash_foreach(value, encode_hash_entry_iter_func, self);
      break;
    default:
      /* raise exception */
      rb_raise(rb_eTypeError, "not valid value");
      break;
  }

  return Qnil;
}

// used above to encode hash entries
int encode_hash_entry_iter_func(VALUE key, VALUE value, VALUE self) {
  cbor_encoder_crt_auto_add(self, key);
  cbor_encoder_crt_auto_add(self, value);
  return ST_CONTINUE;
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
  rb_define_method(cborEncoderClass, "encoded_data", cbor_encoder_get_encoded_data, 0);
  rb_define_method(cborEncoderClass, "add_string", cbor_encoder_add_string, 1);
  rb_define_method(cborEncoderClass, "add_pos_int", cbor_encoder_add_pos_int, 1);
  rb_define_method(cborEncoderClass, "add_neg_int", cbor_encoder_add_neg_int, 1);
  rb_define_method(cborEncoderClass, "add_float", cbor_encoder_add_float, 1);
  rb_define_method(cborEncoderClass, "start_array", cbor_encoder_start_array, 1);
  rb_define_method(cborEncoderClass, "start_map", cbor_encoder_start_map, 1);

  rb_define_method(cborEncoderClass, "crt_auto_add", cbor_encoder_crt_auto_add, 1);

}
