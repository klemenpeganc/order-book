#include <iostream>

#include "Bbo.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Order.h"
#include "OrderBook.h"
#include "Trade.h"

int main() {
  auto fileWriter = FileWriter("output.csv");
  auto orderBook = OrderBook<20, 16384>(
      [&fileWriter](const Trade &trade) {
        fileWriter.writeTradeAcknowledgment(trade.userIdBuy_, trade.userOrderIdBuy_, trade.userIdSell_, trade.userOrderIdSell_,
                                            trade.price_, trade.quantity_);
      },
      [&fileWriter](const Bbo &bbo, char side) {
        if (side == 'B') {
          fileWriter.writeBboChange(side, bbo.bidPrice_, bbo.bidQuantity_);
        } else {
          fileWriter.writeBboChange(side, bbo.askPrice_, bbo.askQuantity_);
        }
      });
  auto fileReader = std::make_unique<FileReader>(
      "inputFile.csv",
      [&orderBook, &fileWriter](int userId, const std::string &symbol, int price, int qty, char side, int userOrderId) {
        fileWriter.writeOrderAcknowledgment(userId, userOrderId);
        orderBook.processNewOrder({userId, userOrderId, symbol, price, qty, side});
      },
      [&orderBook, &fileWriter](int userId, int userOrderId) {
        fileWriter.writeOrderAcknowledgment(userId, userOrderId);
        orderBook.cancelOrder(userId, userOrderId);
      },
      [&orderBook]() { orderBook.flushOrderBook(); });
  return 0;
}