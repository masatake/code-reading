D=$(wildcard *.s)
T=null
M=/usr/share/rst2pdf/aux/Makefile


all: build
	mkdir -p _build
	for x in $D; do \
		mkdir -p _build/$${x:0:1}; \
		for f in $$x/*.pdf; do \
			if ! [[ $$(basename $$f) =~ -answer ]]; then \
				cp -f $$f _build/$${x:0:1}/$$(basename $$f); \
			fi; \
	        done; \
	done

build: symlinks header.rst 
	for x in $(D); do \
		make -C $$x -f $(M) include_DATA=../ChangeLog NAME=$$(basename $$x .s) rst2pdf_OPTIONS="-s $(T)"; \
	done

symlinks:
	for x in $(D); do \
	         for f in $$(grep '^.. include:: '  $$x/$$(basename $$x .s).rst | sed -e 's/^.. include:: //'); do \
			(cd $$x;  ln -f -s $$f $$(basename $$f) || true); \
		done; \
	done
	for x in $(D); do \
	         for f in $$(grep '^.. figure:: '  $$x/$$(basename $$x .s).rst | sed -e 's/^.. figure:: //'); do \
			(cd $$x;  ln -f -s $$f $$(basename $$f) || true); \
		done; \
	done

header.rst: header.rst.in Makefile ChangeLog
	sed -e 's/@DATE@/'"`(date +%Y%m%d-%H:%M)`"'/' < $< > $@

.PHONY: symlinks build

clean:
	rm $$(find . -name '*.pdf')
	rm $$(find . -name 'Makefile.depends')

