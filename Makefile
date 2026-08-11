all: html

.PHONY: clean

# Dependency required: pandoc
MARKDOWN2HTML = pandoc --from gfm --to html --standalone

DOCS := $(patsubst %.md,%.html,$(wildcard *.md proposals/*.md))

html: $(DOCS)

%.html: %.md
	$(MARKDOWN2HTML) $< --output $@ --metadata title="$(shell F=$<; echo $${F%%.*})"

clean:
	rm $(DOCS)
