.PHONY: all poc proposals fritzing build

all: html poc proposals fritzing build
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
ARDUINO_LIBS = AUnit Crypto Embedded_Template_Library_ETL SimpleCLI U8g2 # arduino-NVM

# Add *.cpp files only as DEPS to correctly trigger recompilation on sources change
#   but does not add all sources into APP_SRCS_CPP to avoid compilation troubles for host just right now
# TODO: fix host target compilation for all *.cpp
DEPS = $(shell find src -type f -name '*.cpp' -o -name '*.hpp')
TEST_SRC = $(shell find src -type f -name '*.t.cpp') $(shell find src/t -type f -name '*.cpp')

# TODO: compile object files into separate folder
APP_SRCS_CPP = main.t.cpp $(TEST_SRC) $(patsubst %.t.cpp,%.cpp,$(TEST_SRC))

ETL_FLAGS = -DETL_NO_STL -DETL_NO_INITIALIZER_LIST

# TODO: check is it possible to not define NRF52840_XXAA
COMMON_CFLAGS = -DNRF52840_XXAA -DUSE_TINYUSB $(ETL_FLAGS)
COMMON_CXXFLAGS = -DNRF52840_XXAA -DUSE_TINYUSB $(ETL_FLAGS)

CC=clang
CXX=clang++
# Define EPOXY_DUINO to mark host as target
EXTRA_CFLAGS   += -std=gnu17 $(COMMON_CFLAGS)   -DEPOXY_DUINO -g3
EXTRA_CXXFLAGS += -std=gnu++14 $(COMMON_CXXFLAGS) -DEPOXY_DUINO -g3

# -Wno-main is needed to suppress warning in AUnit <Arduino.h>
EXTRA_CPPFLAGS += $(ETL_FLAGS) -Wno-main

# TODO: decide something with warnings in dependent libraries
# -Wno-unused-but-set-variable is needed to suppress warning in SimpleCLI
# -Wno-#warnings - to suppress warning in Crypto about unknown platform
# -Wno-unused-parameter - to suppress warning in Crypto
# EXTRA_CPPFLAGS += $(ETL_FLAGS) -Wno-main -Wno-unused-but-set-variable -Wno-\#warnings -Wno-unused-parameter

include ../libraries/EpoxyDuino/EpoxyDuino.mk
#
# Build
#
help:  ## Show this help
	@sed -ne '/@sed/!s/:.*## /:\t/p' $(MAKEFILE_LIST)

t: $(APP_NAME).out run

html: $(DOC_FILES) $(PNG_FILES) $(RENDERED_PUML_FILES)

%.html: %.md  ## Generate HTML from Markdown files
	$(MARKDOWN2HTML) $< --output $@ --metadata title="$(shell F=$<; echo $${F%%.*})"

%.png: %.puml  ## Generate PNG pictures from PlantUML schemas
	# Latest PlantUML usage requires as new Java version as available 
	/usr/bin/java -Djava.awt.headless=true -Djava.net.useSystemProxies=true -jar /usr/share/plantuml/plantuml.jar -tpng $<
poc: ## Generate POC docs
	$(MAKE) -C ./poc/

proposals: ## Generate designs docs
	$(MAKE) -C ./proposals/

fritzing:  ## Generate docs at Fritzing related folder
	$(MAKE) -C ./fritzing/

.PHONY: clean_app
clean_app:
	rm -f $(APP_NAME).o $(APP_NAME).out $(patsubst %.cpp,%.o,$(APP_SRCS_CPP)) $(DOC_FILES)
	$(MAKE) -C ./poc/ clean
	$(MAKE) -C ./proposals/ clean

build: build_nrf52840

build_nrf52840: ## Build for target board "Pro Micro NRF52840"
	arduino-cli compile --verbose --log --log-level trace --fqbn "nRFMicro-like-Boards:nrf52:supermini" --build-property "build.extra_flags=$(COMMON_CXXFLAGS)"

rebuild_nrf52840: ## Force rebuild for target board "Pro Micro NRF52840"
	arduino-cli compile --clean --verbose --log --log-level trace --fqbn "nRFMicro-like-Boards:nrf52:supermini" --build-property "build.extra_flags=$(COMMON_CXXFLAGS)"

build_avr: ## Build for target board Arduino Uno
	arduino-cli compile --fqbn arduino:avr:uno

compiler_commands.json: $(DEPS) clean ## Generate compilation database
	compiledb -n make t
	# arduino-cli compile --fqbn "nRFMicro-like-Boards:nrf52:supermini" --only-compilation-database --build-path ./build --build-property "build.extra_flags=$(COMMON_CXXFLAGS)"

dt:  ## Run GDB to debug unit tests
	gdb -tui $(APP_NAME).out
