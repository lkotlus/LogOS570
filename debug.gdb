set confirm off
add-symbol-file build/kernelfull.o 0x100000
target remote localhost:1234
set confirm on
