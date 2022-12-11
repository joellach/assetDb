CC = g++
CFLAGS = -g -Wall
TARGET = assetDb

all: $(TARGET) test

test: 
	./create_asset_csv.py
	./test_asset_csv.py
clean:
	rm ./assetDb assetDb.csv

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp
