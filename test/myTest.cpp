#include "Bbo.h"
#include "Order.h"
#include "OrderBook.h"
#include "Trade.h"
#include "gtest/gtest.h"
TEST(OrderBookTest, scenario1) {
  Trade expectedTrade;
  Bbo expectedBbo;
  auto orderBook = OrderBook<20, 10819>([&expectedTrade](const Trade &incomingTrade) { EXPECT_EQ(expectedTrade, incomingTrade); },
                                        [&expectedBbo](const Bbo &bbo, char side) {});
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 10, .qty_ = 100, .side_ = 'B', .userOrderId_ = 1};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 12, .qty_ = 100, .side_ = 'S', .userOrderId_ = 2};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 2, .symbol_ = "IBM", .price_ = 9, .qty_ = 100, .side_ = 'B', .userOrderId_ = 101};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 2, .symbol_ = "IBM", .price_ = 11, .qty_ = 100, .side_ = 'S', .userOrderId_ = 102};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 11, .qty_ = 100, .side_ = 'B', .userOrderId_ = 3};
    expectedTrade = Trade{.userIdBuy_ = 1, .userOrderIdBuy_ = 3, .userIdSell_ = 2, .userOrderIdSell_ = 102, .price_ = 11, .quantity_ = 100};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 2, .symbol_ = "IBM", .price_ = 10, .qty_ = 100, .side_ = 'S', .userOrderId_ = 103};
    expectedTrade = Trade{.userIdBuy_ = 1, .userOrderIdBuy_ = 1, .userIdSell_ = 2, .userOrderIdSell_ = 103, .price_ = 10, .quantity_ = 100};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 10, .qty_ = 100, .side_ = 'B', .userOrderId_ = 4};
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 2, .symbol_ = "IBM", .price_ = 11, .qty_ = 100, .side_ = 'S', .userOrderId_ = 104};
    orderBook.processNewOrder(order);
  }
}

TEST(OrderBookTest, bboUpdate) {
  Bbo expectedBbo;
  auto orderBook = OrderBook<20, 15000>([](const Trade &incomingTrade) {}, [&expectedBbo](const Bbo &bbo, char side) { EXPECT_EQ(expectedBbo, bbo); });
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 10, .qty_ = 100, .side_ = 'S', .userOrderId_ = 1};
    expectedBbo.askPrice_ = 10;
    expectedBbo.askQuantity_ = 100;
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 10, .qty_ = 55, .side_ = 'S', .userOrderId_ = 2};
    expectedBbo.askQuantity_ = 155;
    orderBook.processNewOrder(order);
  }
  {
    auto order = Order{.userId_ = 2, .symbol_ = "IBM", .price_ = 10, .qty_ = 75, .side_ = 'B', .userOrderId_ = 2};
    expectedBbo.askQuantity_ = 80;
    orderBook.processNewOrder(order);
  }
}

TEST(OrderBookTest, cancelOrder) {
  auto orderBook = OrderBook<20, 1000>([](const Trade &trade) {}, [](const Bbo &bbo, char side) {});
  {
    auto order = Order{.userId_ = 1, .symbol_ = "IBM", .price_ = 10, .qty_ = 100, .side_ = 'B', .userOrderId_ = 1};
    orderBook.processNewOrder(order);
  }
  orderBook.cancelOrder(1, 1);
}