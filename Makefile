BOARD   := esp32:esp32:esp32
SKETCH  := hot-dog.ino
PORT    ?= /dev/ttyUSB0
BAUD    ?= 115200

BUILD_DIR := /tmp/hot-dog-build

.PHONY: all compile upload monitor install-libs clean

all: compile upload monitor

compile:
	arduino-cli compile \
		--fqbn $(BOARD) \
		--build-path $(BUILD_DIR) \
		$(SKETCH)

upload:
	arduino-cli upload \
		--fqbn $(BOARD) \
		--port $(PORT) \
		--input-dir $(BUILD_DIR) \
		$(SKETCH)

monitor:
	arduino-cli monitor --port $(PORT) --config baudrate=$(BAUD)

install-libs:
	arduino-cli lib install "DHT sensor library"
	arduino-cli lib install "Adafruit GFX Library"
	arduino-cli lib install "Adafruit SSD1306"
	arduino-cli lib install "UniversalTelegramBot"

clean:
	rm -rf $(BUILD_DIR)
