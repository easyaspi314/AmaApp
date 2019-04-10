CC := gcc
CFLAGS := -Wall -Wpedantic -std=c99 -O2
LDFLAGS :=
C_FILES := AmaApp.c Date.c Product.c main.c Utilities.c
H_FILES := AmaApp.h Date.h Product.h Utilities.h
OBJ_FILES := $(C_FILES:.c=.o)
ifeq ($(OS),Windows_NT)
EXT := .exe
else
EXT :=
endif

AmaApp$(EXT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	cp -f inventory.txt.orig inventory.txt

$(OBJ_FILES): %.o: %.c $(H_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJ_FILES) AmaApp$(EXT)
.PHONY: clean
