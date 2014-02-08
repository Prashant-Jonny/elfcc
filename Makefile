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

.PHONY: test clean

clean:
	rm *.o $(TARGET)

test:
	rm -rf test
	mkdir test
	cp $(TARGET) test/$(TARGET)
	./$(TARGET) -d test/$(TARGET) test/$(TARGET)
	./$(TARGET) -c test/$(TARGET).elfs test/$(TARGET).new
	readelf -a test/$(TARGET) > test/$(TARGET).re
	readelf -a test/$(TARGET).new > test/$(TARGET).new.re
	xxd test/$(TARGET) > test/$(TARGET).hex
	xxd test/$(TARGET).new > test/$(TARGET).new.hex
	diff test/$(TARGET) test/$(TARGET).new