CC = g++
CFLAGS = -g -Wall
TARGET = assetDb

all: $(TARGET) test

test: 
	./run_test.py
clean:
	rm ./assetDb assetDb.csv

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp
