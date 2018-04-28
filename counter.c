#include <ruby.h>

static const rb_data_type_t counter_type = {
    "counter",
    {NULL, NULL, NULL,},
    NULL, NULL,
    RUBY_TYPED_FREE_IMMEDIATELY|RUBY_TYPED_WB_PROTECTED
};

static VALUE
counter_alloc(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &counter_type, (void *)INT2FIX(0));
}

static VALUE
counter_init(VALUE self, VALUE num)
{
    RB_OBJ_WRITE(self, &DATA_PTR(self), rb_to_int(num));
    return self;
}

static VALUE
counter_to_i(VALUE self)
{
    return (VALUE)DATA_PTR(self);
}

#ifndef HAVE_RB_INT_SUCC
static VALUE
rb_int_succ(VALUE num)
{
    if (FIXNUM_P(num)) {
	long i = FIX2LONG(num) + 1;
	return LONG2NUM(i);
    }
    if (RB_TYPE_P(num, T_BIGNUM)) {
	return rb_big_plus(num, INT2FIX(1));
    }
    return rb_funcall(num, '+', 1, INT2FIX(1));
}
#endif

#ifndef HAVE_RB_INT_PRED
static VALUE
rb_int_pred(VALUE num)
{
    if (FIXNUM_P(num)) {
	long i = FIX2LONG(num) - 1;
	return LONG2NUM(i);
    }
    if (RB_TYPE_P(num, T_BIGNUM)) {
	return rb_big_minus(num, INT2FIX(1));
    }
    return rb_funcall(num, '-', 1, INT2FIX(1));
}
#endif

static VALUE
counter_succ_bang(VALUE self)
{
    VALUE num = counter_to_i(self);
    num = rb_int_succ(num);
    counter_init(self, num);
    return self;
}

static VALUE
counter_pred_bang(VALUE self)
{
    VALUE num = counter_to_i(self);
    num = rb_int_pred(num);
    counter_init(self, num);
    return self;
}

static VALUE
counter_coerce(VALUE self, VALUE other)
{
    return rb_assoc_new(other, counter_to_i(self));
}

static VALUE
counter_to_s(VALUE self)
{
    return rb_sprintf("#<%"PRIsVALUE":%"PRIsVALUE">", rb_obj_class(self), counter_to_i(self));
}

static VALUE
counter_respond_to_missing(VALUE self, VALUE mid, VALUE priv)
{
    ID id = rb_check_id(&mid);
    if (!id) return Qfalse;
    return rb_obj_respond_to(counter_to_i(self), id, RTEST(priv));
}

static VALUE
counter_method_missing(int argc, VALUE *argv, VALUE self)
{
    VALUE mid;
    ID id;
    rb_check_arity(argc, 1, UNLIMITED_ARGUMENTS);
    mid = argv[0];
    id = rb_check_id(&mid);
    if (!id) return rb_call_super(argc, argv);
    return rb_funcallv(counter_to_i(self), id, argc-1, argv+1);
}

void
Init_counter(void)
{
    VALUE cCounter = rb_define_class("Counter", rb_cNumeric);
    rb_define_alloc_func(cCounter, counter_alloc);
    rb_define_method(cCounter, "initialize", counter_init, 1);
    rb_define_method(cCounter, "to_int", counter_to_i, 0);
    rb_define_method(cCounter, "to_i", counter_to_i, 0);
    rb_define_method(cCounter, "to_s", counter_to_s, 0);
    rb_define_method(cCounter, "inspect", counter_to_s, 0);
    rb_define_method(cCounter, "coerce", counter_coerce, 1);
    rb_define_method(cCounter, "succ!", counter_succ_bang, 0);
    rb_define_method(cCounter, "pred!", counter_pred_bang, 0);
    rb_define_method(cCounter, "respond_to_missing?", counter_respond_to_missing, 2);
    rb_define_method(cCounter, "method_missing", counter_method_missing, -1);
}
