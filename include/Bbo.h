#pragma once

struct Bbo {
  unsigned int bidPrice_ = 0;
  unsigned int bidQuantity_ = 0;
  unsigned int askPrice_ = 0;
  unsigned int askQuantity_ = 0;
  bool operator==(const Bbo &rhs) const {
    return bidPrice_ == rhs.bidPrice_ && bidQuantity_ == rhs.bidQuantity_ && askPrice_ == rhs.askPrice_ && askQuantity_ == rhs.askQuantity_;
  }
};