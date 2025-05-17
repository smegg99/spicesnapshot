PKG_CFG := pkg-config
CFLAGS  := -Wall -Wextra -O2 $(shell $(PKG_CFG) --cflags spice-client-gtk-3.0)
LDFLAGS := $(shell $(PKG_CFG) --libs   spice-client-gtk-3.0)

SRCS    := main.c capture.c
TARGET  := spicesnapshot

ARCHS   := amd64 arm64
OS      := linux

VERSION := $(shell git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0")

DISTDIR := dist

.PHONY: all dist clean

all: dist

dist: | $(DISTDIR)
	@for arch in $(ARCHS); do \
	  case $$arch in \
	    amd64) CC=gcc ;; \
	    arm64) CC=aarch64-linux-gnu-gcc ;; \
	    *) echo "Unknown arch $$arch, skipping"; continue ;; \
	  esac; \
	  if ! command -v $$CC >/dev/null 2>&1; then \
	    echo "Skipping $$arch: $$CC not found"; \
	    continue; \
	  fi; \
	  echo "Building $(TARGET) for $(OS)/$$arch..."; \
	  $$CC $(CFLAGS) -o $(DISTDIR)/$(TARGET)-$(OS)-$$arch-$(VERSION) $(SRCS) $(LDFLAGS); \
	  echo "Creating archive: $(TARGET)-$(OS)-$$arch-$(VERSION).tar.gz"; \
	  tar -C $(DISTDIR) -czf $(DISTDIR)/$(TARGET)-$(OS)-$$arch-$(VERSION).tar.gz \
	    $(TARGET)-$(OS)-$$arch-$(VERSION); \
	done
	@echo "Done. Packages in $(DISTDIR)/"

$(DISTDIR):
	mkdir -p $(DISTDIR)

clean:
	rm -rf $(DISTDIR)
