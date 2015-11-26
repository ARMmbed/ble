# Makefile used to generate the API documentation using Doxygen

.PHONY: apidoc apidoc_clean

all: apidoc

apidoc:
	mkdir -p apidoc
	doxygen ble.doxyfile

apidoc_clean:
	rm -rf apidoc
