#pragma once

struct OrderBookEntry {
  OrderBookEntry *next_;
  int size_;
  int userId_;
  int userOrderId_;
};