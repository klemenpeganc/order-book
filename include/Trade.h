#pragma once

struct Trade {
  int userIdBuy_;
  int userOrderIdBuy_;
  int userIdSell_;
  int userOrderIdSell_;
  unsigned int price_;
  int quantity_;
  bool operator==(const Trade &rhs) const {
    return userIdBuy_ == rhs.userIdBuy_ && userOrderIdBuy_ == rhs.userOrderIdBuy_ && userIdSell_ == rhs.userIdSell_ &&
           userOrderIdSell_ == rhs.userOrderIdSell_ && price_ == rhs.price_ && quantity_ == rhs.quantity_;
  };
};