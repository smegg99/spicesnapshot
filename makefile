CC      := gcc
PKG_CFG := pkg-config
PKG_CFLAGS := $(PKG_CFG) --cflags spice-client-gtk-3.0
PKG_LIBS   := $(PKG_CFG) --libs   spice-client-gtk-3.0

CFLAGS  := -Wall -Wextra -O2 $(shell $(PKG_CFLAGS))
LDFLAGS := $(shell $(PKG_LIBS))

SRCS    := main.c capture.c
OBJS    := $(SRCS:.c=.o)
TARGET  := spicesnapshot

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c capture.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)
