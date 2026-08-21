all: html

.PHONY: clean

# Dependency required: pandoc
MARKDOWN2HTML = pandoc --from gfm --to html --standalone

DOC_FILES = $(patsubst %.md,%.html,$(wildcard *.md))

PUML_FILES = $(wildcard *.puml)
RENDERED_PUML_FILES = $(patsubst %.puml,%.png,$(PUML_FILES))

PNG_FILES = $(wildcard *.png)

html: $(DOC_FILES) $(PNG_FILES) $(RENDERED_PUML_FILES)

%.html: %.md
	$(MARKDOWN2HTML) $< --output $@ --metadata title="$(shell F=$<; echo $${F%%.*})"

%.png: %.puml
	# Latest PlantUML usage requires as new Java version as available 
	/usr/bin/java -Djava.awt.headless=true -Djava.net.useSystemProxies=true -jar /usr/share/plantuml/plantuml.jar -tpng $<

clean:
	rm $(DOC_FILES)

