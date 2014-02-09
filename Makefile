CC = gcc
CFLAGS = -O2
SOURCES = writer.c reader.c elfcc.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = elfcc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: test test_elfcc test_ls clean

clean:
	rm *.o $(TARGET)

test: test_elfcc test_ls

test_elfcc:
	rm -rf test
	mkdir -p test/$(TARGET)
	cp $(TARGET) test/$(TARGET)/$(TARGET)
	./$(TARGET) -d test/$(TARGET)/$(TARGET)
	./$(TARGET) -c test/$(TARGET)/$(TARGET) test/$(TARGET)/$(TARGET).new
	readelf -a test/$(TARGET)/$(TARGET) > test/$(TARGET)/$(TARGET).re
	readelf -a test/$(TARGET)/$(TARGET).new > test/$(TARGET)/$(TARGET).new.re
	xxd test/$(TARGET)/$(TARGET) > test/$(TARGET)/$(TARGET).hex
	xxd test/$(TARGET)/$(TARGET).new > test/$(TARGET)/$(TARGET).new.hex

test_ls:
	mkdir -p test/ls
	cp /bin/ls test/ls/ls
	./$(TARGET) -d test/ls/ls
	./$(TARGET) -c test/ls/ls test/ls/ls.new
	readelf -a test/ls/ls > test/ls/ls.re
	readelf -a test/ls/ls.new > test/ls/ls.new.re
	xxd test/ls/ls > test/ls/ls.hex
	xxd test/ls/ls.new > test/ls/ls.new.hex
