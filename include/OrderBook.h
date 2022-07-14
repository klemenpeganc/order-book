#pragma once
#include "Bbo.h"
#include "Order.h"
#include "OrderBookEntry.h"
#include "PriceBucket.h"
#include "Trade.h"
#include <array>
#include <iostream>
#include <memory>
namespace {
unsigned int pairingFunction(int a, int b) { return a >= b ? a * a + a + b : a + b * b; }
} // namespace

template <unsigned int MAX_PRICE, unsigned int MAX_ORDERS> class OrderBook {
  using on_trade_callback_t = std::function<void(const Trade &trade)>;
  using on_bbo_update_callback_t = std::function<void(const Bbo &bbo, char side)>;

public:
  OrderBook(on_trade_callback_t onTradeCallback, on_bbo_update_callback_t onBboUpdateCallback)
      : onTradeCallback_(std::move(onTradeCallback)), onBboUpdateCallback_(std::move(onBboUpdateCallback)) {

    // initialize the arrays to hold all possible prices and orders to avoid dynamic memory
    memset(&allOrders_, 0, MAX_ORDERS * sizeof(OrderBookEntry));
    memset(&priceBuckets_, 0, (MAX_PRICE + 1) * sizeof(PriceBucket));
  }

  void processNewOrder(const Order &order) {
    OrderBookEntry *existingOrder;
    PriceBucket *existingPriceBucket;
    auto orderPrice = order.price_;
    auto orderSize = order.qty_;
    bool track_bbo = false;
    double askMinCurr = askMin_;
    // new order to buy
    if (order.side_ == 'B') {
      existingPriceBucket = priceBuckets_.data() + askMin_;
      // check if there is something to buy
      while (orderPrice >= askMin_) {
        // get to the price bucket and its orders and get the oldest order
        existingOrder = existingPriceBucket->oldestOrder;
        while (existingOrder != nullptr) {
          // partial fill from an existing order
          if (existingOrder->size_ < orderSize) {
            onTradeCallback_(
                {order.userId_, order.userOrderId_, existingOrder->userId_, existingOrder->userOrderId_, askMin_, existingOrder->size_});
            bbo_.askQuantity_ -= existingOrder->size_;
            orderSize -= existingOrder->size_;
            // get the next order in the price bucket and fill from there if it exists
            existingOrder = existingOrder->next_;
          }
          // we can fill the full order
          else {
            onTradeCallback_({order.userId_, order.userOrderId_, existingOrder->userId_, existingOrder->userOrderId_, askMin_, orderSize});

            if (existingOrder->size_ > orderSize) {
              existingOrder->size_ -= orderSize;
            } else {
              // we filled whole of the existing order, point it to the next in line
              existingOrder = existingOrder->next_;
            }
            bbo_.askQuantity_ -= orderSize;
            onBboUpdateCallback_(bbo_, 'S');
            // the oldest order either stays the same one with less size or it becomes the next one in the price bucket
            existingPriceBucket->oldestOrder = existingOrder;
            return;
          }
        }

        // nothing more available at current price, move one level higher
        existingPriceBucket->oldestOrder = nullptr;
        ++existingPriceBucket;
        ++askMin_;
        onBboUpdateCallback_(bbo_, 'S');
      }
      // couldn't fill the whole buy order, put it in the order book at the remaining size
      OrderBookEntry *newOrder = allOrders_.data() + pairingFunction(order.userId_, order.userOrderId_);
      newOrder->size_ = orderSize;
      newOrder->userId_ = order.userId_;
      newOrder->userOrderId_ = order.userOrderId_;
      insertNewOrder(&priceBuckets_[orderPrice], newOrder);
      if (bidMax_ < orderPrice) {
        bidMax_ = orderPrice;
        bbo_.bidPrice_ = orderPrice;
        bbo_.bidQuantity_ = orderSize;
        onBboUpdateCallback_(bbo_, 'B');
      } else if (bidMax_ == orderPrice) {
        bbo_.bidQuantity_ += orderSize;
        onBboUpdateCallback_(bbo_, 'B');
      }
    } else {
      // get the price bucket at the top of the bids
      existingPriceBucket = priceBuckets_.data() + bidMax_;
      while (orderPrice <= bidMax_) {
        // get the oldest order for that price
        existingOrder = existingPriceBucket->oldestOrder;
        while (existingOrder != nullptr) {
          // partial fill
          if (existingOrder->size_ < orderSize) {
            onTradeCallback_(
                {existingOrder->userId_, existingOrder->userOrderId_, order.userId_, order.userOrderId_, bidMax_, existingOrder->size_});
            orderSize -= existingOrder->size_;
            bbo_.bidQuantity_ -= existingOrder->size_;
            existingOrder = existingOrder->next_;
          } else {
            // (rest of the) order fully filled
            onTradeCallback_({existingOrder->userId_, existingOrder->userOrderId_, order.userId_, order.userOrderId_, bidMax_, orderSize});
            if (existingOrder->size_ > orderSize) {
              existingOrder->size_ -= orderSize;
            } else {
              existingOrder = existingOrder->next_;
            }

            bbo_.bidQuantity_ -= orderSize;
            onBboUpdateCallback_(bbo_, 'B');
            // the oldest order either stays the same one with less size or it becomes the next one in the price bucket
            existingPriceBucket->oldestOrder = existingOrder;
            return;
          }
        }

        // nothing else available at this price, check if we have something at a worse price, but still within the user entered price
        existingPriceBucket->oldestOrder = nullptr;
        --existingPriceBucket;
        --bidMax_;
        onBboUpdateCallback_(bbo_, 'B');
      }

      // nothing (else) exists for this price, so place the order in the orderbook with the (remaining) quantity.
      OrderBookEntry *newOrder = allOrders_.data() + pairingFunction(order.userId_, order.userOrderId_);
      newOrder->size_ = orderSize;
      newOrder->userId_ = order.userId_;
      newOrder->userOrderId_ = order.userOrderId_;
      insertNewOrder(&priceBuckets_[orderPrice], newOrder);
      if (askMin_ > orderPrice) {
        askMin_ = orderPrice;
        bbo_.askPrice_ = orderPrice;
        bbo_.askQuantity_ = orderSize;
        onBboUpdateCallback_(bbo_, 'S');
      } else if (askMin_ == orderPrice) {
        bbo_.askQuantity_ += orderSize;
        onBboUpdateCallback_(bbo_, 'S');
      }
    }
  }

  void cancelOrder(int userId, int userOrderId) {
    // to save on time complexity, just set the size to 0 and it will be skipped.
    allOrders_[pairingFunction(userId, userOrderId)].size_ = 0;
  }

  void flushOrderBook() {
    askMin_ = MAX_PRICE + 1;
    bidMax_ = 0;
    memset(&allOrders_, 0, MAX_ORDERS * sizeof(OrderBookEntry));
    memset(&priceBuckets_, 0, (MAX_PRICE + 1) * sizeof(PriceBucket));
  }

private:
  void insertNewOrder(PriceBucket *priceBucket, OrderBookEntry *newOrder) {
    // if there aren't any orders for the specific price, put it as the oldest, otherwise put it in the last place in line
    if (priceBucket->oldestOrder == nullptr) {
      priceBucket->oldestOrder = newOrder;
    } else {
      priceBucket->newestOrder->next_ = newOrder;
    }
    priceBucket->newestOrder = newOrder;
  }

  unsigned int askMin_ = MAX_PRICE + 1;
  unsigned int bidMax_ = 0;
  std::array<PriceBucket, MAX_PRICE + 1> priceBuckets_;
  std::array<OrderBookEntry, MAX_ORDERS> allOrders_;
  on_trade_callback_t onTradeCallback_;
  on_bbo_update_callback_t onBboUpdateCallback_;
  Bbo bbo_;
};
