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
# Unit Tests
# They will be compiled for host target.
# BTW, main code will be build by Arduino IDE cross compiler framework in separate folder.
#   See for example makefile target "build_nrf52840"

APP_NAME := CredsHolder

# Add all dependency libraries into ARDUINO_LIBS to allow AUnit add consequent "-I..." compiler flags
# and make includes working for that libraries
ARDUINO_LIBS = AUnit Crypto Embedded_Template_Library_ETL SimpleCLI # arduino-NVM

# Add *.cpp files only as DEPS to correctly trigger recompilation on sources change
#   but does not add all sources into APP_SRCS_CPP to avoid compilation troubles for host just right now
# TODO: fix host target compilation for all *.cpp
DEPS = $(shell find src -type f -name '*.cpp')

# TODO: compile object files into separate folder
APP_SRCS_CPP = main.t.cpp $(shell find src -type f -name '*.t.cpp')

# Define EPOXY_DUINO to mark host as target
# TODO: check is it possible to not define NRF52840_XXAA
EXTRA_CFLAGS   += -DEPOXY_DUINO -g3 -DNRF52840_XXAA -DUSE_TINYUSB -DETL_NO_STL -DETL_NO_INITIALIZER_LIST
EXTRA_CPPFLAGS += -DEPOXY_DUINO -g3 -DNRF52840_XXAA -DUSE_TINYUSB -DETL_NO_STL -DETL_NO_INITIALIZER_LIST
EXTRA_CXXFLAGS += -DEPOXY_DUINO -g3 -DNRF52840_XXAA -DUSE_TINYUSB -DETL_NO_STL -DETL_NO_INITIALIZER_LIST

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
	rm -f $(APP_NAME).o $(APP_NAME).out $(patsubst %.cpp,%.o,$(APP_SRCS_CPP)) $(DOC_FILES)
	$(MAKE) -C ./poc/ clean
	$(MAKE) -C ./proposals/ clean

build: build_nrf52840

build_nrf52840: ## Target board is Pro Micro NRF52840
	arduino-cli compile --verbose --log --log-level trace --fqbn "nRFMicro-like-Boards:nrf52:supermini" --build-property "build.extra_flags=-I$$(pwd)/include -DNRF52840_XXAA -DCFG_TUD_ENABLED=1 -DCFG_TUD_HID=1 -DUSE_TINYUSB=1 -DETL_NO_STL -DETL_NO_INITIALIZER_LIST"

build_avr: ## Target board is Arduino Uno
	arduino-cli compile -b arduino:avr:uno

dt:
	gdb -tui $(APP_NAME).out
