/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "sbus.hpp"
#include <iostream>
#include <memory.h>
#include <stdio.h>

/// \tag::hello_uart[]

#define UART_ID uart1
#define BAUD_RATE 100000

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

/**
 * UART packet documentation
 * Start bytes 0xAF 0xCF
 * 4 bytes CH0 start idx 0
 * 4 bytes CH1 start idx 4
 * 4 bytes CH2 start idx 8
 * 4 bytes CH3 start idx 12
 * 1 byte CH4 (enable) start idx 16
 * End bytes 0xDF 0xBF
 * Total packet length 21 bytes
 **/

struct CommandHandler {
  SbusTx sbus;
  const char startBytes[2] = {0xAF, 0xCF};
  const char endBytes[2] = {0xDF, 0xBF};
  const size_t packetLength = 21; // 2 start + 17 data + 2 end
  char dataBuf[17];
  bool startByteOK = false;
  size_t currentIdx = 0;
  size_t discardedPackets = 0;
  bool packetReady = false;
  bool sbusReady = false;

  int convert4BytesToInt(size_t start) {
    char temp[5];
    memcpy(temp, &dataBuf[start], 4);
    temp[4] = 0;
    printf(temp);
    return atoi(temp);
  }

  void updateSbus() {
    sbus.ch_[0] = convert4BytesToInt(0);
    sbus.ch_[1] = convert4BytesToInt(4);
    sbus.ch_[2] = convert4BytesToInt(8);
    sbus.ch_[3] = convert4BytesToInt(12);
    if (dataBuf[16] != 0) {
      sbus.ch_[4] = 2000;
    } else {
      sbus.ch_[4] = 500;
    }
    sbus.updateBuffer();
  }

  void handleInput(char c) {
    // Get start bytes
    if (c == startBytes[0]) {
      if (currentIdx == 0) {
        currentIdx++;
        return;
      } else {
        discardedPackets++;
        startByteOK = false;
        return;
      }
    } else if (c == startBytes[1]) {
      if (currentIdx == 1) {
        currentIdx++;
        startByteOK = true;
        return;
      } else {
        discardedPackets++;
        currentIdx = 0;
        startByteOK = false;
        return;
      }
    }
    // Check end bytes
    else if (c == endBytes[0]) {
      if (currentIdx == (packetLength - 2)) {
        currentIdx++;
      } else {
        currentIdx = 0;
        discardedPackets++;
        startByteOK = false;
        return;
      }
    } else if (c == endBytes[1]) {
      if (currentIdx == (packetLength - 1)) {
        currentIdx = 0;
        packetReady = true;
      } else {
        currentIdx = 0;
        discardedPackets++;
        startByteOK = false;
        return;
      }
    } else if (startByteOK && currentIdx >= 2 && currentIdx < 19) {
      dataBuf[currentIdx - 2] = c;
      currentIdx++;
    }

    if (packetReady) {
      updateSbus();
      currentIdx = 0;
      startByteOK = false;
      packetReady = false;
      return;
    }
  }
};

CommandHandler cmdHandler;
bool sbusCallback(repeating_timer_t *rt) {
  uart_write_blocking(UART_ID, cmdHandler.sbus.buf_, 25);
  return true;
}

int main() {
  stdio_init_all();

  // Set up our UART with the required speed.

  uart_init(UART_ID, BAUD_RATE);
  uart_set_format(UART_ID, 8, 2, UART_PARITY_EVEN);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  gpio_set_outover(UART_TX_PIN, GPIO_OVERRIDE_INVERT);
  repeating_timer_t timer;
  if (add_repeating_timer_ms(15, sbusCallback, NULL, &timer)) {
    printf("Created SBUS timer\n");
  }
  // Use some the various UART functions to send out data
  // In a default system, printf will also output via the default UART

  while (1) {
    int c = getchar_timeout_us(0);
    if (c >= 0) {
      // printf("Got %c\n", c);
      cmdHandler.handleInput((char)c);
    }
    sleep_us(200);

    // sleep_ms(15); // Need to get rid of this when command handler is ready,
    //               // otherwise its blocking the getchar
    // value++;
    // if (value > 1500) {
    //   value = 1000;
    // }
  }
}

/// \end::hello_uart[]
