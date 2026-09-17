all: html poc proposals build

.PHONY: poc proposals fritzing build
#
# Documentation
#
# Dependency required: pandoc
MARKDOWN2HTML = pandoc --from gfm --to html --standalone

DOC_FILES = $(patsubst %.md,%.html,$(wildcard *.md))

PUML_FILES = $(wildcard *.puml)
RENDERED_PUML_FILES = $(patsubst %.puml,%.png,$(PUML_FILES))

PNG_FILES = $(wildcard *.png)
#
# Sources
#
APP_NAME := CredsHolder
ARDUINO_LIBS := AUnit Crypto
DEPS := $(wildcard src/*.cpp)
APP_SRCS_CPP := main.t.cpp $(wildcard src/*.t.cpp)

EXTRA_CFLAGS := -g3
EXTRA_CPPFLAGS := -g3
EXTRA_CXXFLAGS := -g3

include ../libraries/EpoxyDuino/EpoxyDuino.mk
#
# Build
#
t: $(APP_NAME).out run

html: $(DOC_FILES) $(PNG_FILES) $(RENDERED_PUML_FILES)

%.html: %.md
	$(MARKDOWN2HTML) $< --output $@ --metadata title="$(shell F=$<; echo $${F%%.*})"

%.png: %.puml
	# Latest PlantUML usage requires as new Java version as available 
	/usr/bin/java -Djava.awt.headless=true -Djava.net.useSystemProxies=true -jar /usr/share/plantuml/plantuml.jar -tpng $<
poc:
	$(MAKE) -C ./poc/

proposals:
	$(MAKE) -C ./proposals/

fritzing:
	$(MAKE) -C ./fritzing/

.PHONY: clean_app
clean_app:
	rm $(wildcard *.t.o) $(wildcard src/*.t.o)
	for f in "$(DOC_FILES)"; do if [ -f "$$f" ]; then rm "$$f"; fi; done
	$(MAKE) -C ./poc/ clean
	$(MAKE) -C ./proposals/ clean

# Current target board is Pro Micro NRF52840
build: build_nrf52840

build_nrf52840:
	arduino-cli compile -b nRFMicro-like-Boards:nrf52:supermini

build_avr:
	arduino-cli compile -b arduino:avr:uno
