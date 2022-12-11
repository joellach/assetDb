CC = g++
CFLAGS = -g -Wall
TARGET = assetDb

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp