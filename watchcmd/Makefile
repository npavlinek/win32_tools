srcs = \
	watchcmd.c \
	win32.c
objs = $(srcs:%.c=%.o)
depends = $(objs:.o=.d)

cflags ?= \
	-O2 \
	-Weverything \
	-Wno-c99-compat \
	-Wno-long-long \
	-fuse-ld=lld-link \
	-pedantic \
	-std=c89

watchcmd.exe: fixed_width_integers.h win32.h $(objs)
	clang $(cflags) $(objs) -o $@

fixed_width_integers.h: gen_fixed_width_integers.rb
	ruby $< > gen_fixed_width_integers.c
	clang -o gen_fixed_width_integers.exe gen_fixed_width_integers.c
	gen_fixed_width_integers.exe > $@
	del /Q gen_fixed_width_integers.c gen_fixed_width_integers.exe

win32.c: gen_win32.rb
	ruby $< impl > $@
win32.h: gen_win32.rb
	ruby $< header > $@

%.o: %.c
	clang $(cflags) -MMD -MP -o $@ -c $<

-include $(depends)

install: watchcmd.exe
	copy /Y watchcmd.exe ..\bin\

clean:
	del $(objs) $(depends) watchcmd.exe

.PHONY: install clean
