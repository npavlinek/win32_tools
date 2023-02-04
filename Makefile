install:
	make -C etime $@
	make -C watchcmd $@

clean:
	make -C etime $@
	make -C watchcmd $@

.PHONY: install clean
