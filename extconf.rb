require 'mkmf'

have_func("rb_int_succ")
have_func("rb_int_pred")

create_makefile('counter')
