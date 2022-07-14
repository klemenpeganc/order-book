#include "FileReader.h"
#include <iostream>
#include <sstream>
#include <vector>
FileReader::FileReader(std::string_view fileName, on_new_order_callback_t onNewOrderInputCallback,
                       on_cancel_order_callback_t onCancelOrderInputCallback, on_flush_orderbook_callback_t onFlushOrderBookCallback)
    : fileStream_(fileName), onNewOrderInputCallback_(std::move(onNewOrderInputCallback)),
      onCancelOrderInputCallback_(std::move(onCancelOrderInputCallback)), onFlushOrderBookCallback_(std::move(onFlushOrderBookCallback)) {
  while (fileStream_) {
    std::string inputLine;
    if (!getline(fileStream_, inputLine)) {
      break;
    }
    const char decidingChar = inputLine.front();

    if (decidingChar == '#') {
      continue;
    }
    std::istringstream stringStream(inputLine);
    stringStream.ignore(2);
    if (decidingChar == 'N') {
      int userId, price, qty, userOrderId;
      std::string symbol;
      char side;
      char delimiter;
      stringStream >> userId >> delimiter >> symbol >> price >> delimiter >> qty >> delimiter >> side >> delimiter >> userOrderId;
      symbol.pop_back(); // to ignore the ',' when reading in the symbol as a string
      onNewOrderInputCallback_(userId, symbol, price, qty, side, userOrderId);
    } else if (decidingChar == 'C') {
      int userId, userOrderId;
      char delimiter;
      stringStream >> userId >> delimiter >> userOrderId;
      onCancelOrderInputCallback_(userId, userOrderId);
    } else if (decidingChar == 'F') {
      onFlushOrderBookCallback_();
    }
  }
}
