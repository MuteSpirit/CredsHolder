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

EXTRA_CFLAGS := -g3 -DNRF52840
EXTRA_CPPFLAGS := -g3 -DNRF52840
EXTRA_CXXFLAGS := -g3 -DNRF52840

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
	for f in "$(APP_NAME).o  $(APP_NAME).out  $(patsubst %.cpp,%.o,$(APP_SRCS_CPP))"; do if [ -f "$$f" ]; then rm "$$f"; fi; done
	for f in "$(DOC_FILES)"; do if [ -f "$$f" ]; then rm "$$f"; fi; done
	$(MAKE) -C ./poc/ clean
	$(MAKE) -C ./proposals/ clean

# Current target board is Pro Micro NRF52840
build: build_nrf52840

build_nrf52840:
	# arduino-cli compile --verbose --log --log-level debug --fqbn "nRFMicro-like-Boards:nrf52:supermini" --build-property "build.extra_flags=-I$$(pwd)/include -DNRF52 -DNRF52840_XXAA -DNRF52840 -DCFG_TUD_ENABLED=1 -DCFG_TUD_HID=1 -DUSE_TINYUSB=1"
	arduino-cli compile --verbose --log --log-level debug --fqbn "nRFMicro-like-Boards:nrf52:supermini" --build-property "build.extra_flags=-I$$(pwd)/include -DNRF52840_XXAA -DCFG_TUD_ENABLED=1 -DCFG_TUD_HID=1 -DUSE_TINYUSB=1"

build_avr:
	arduino-cli compile -b arduino:avr:uno
