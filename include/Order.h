#pragma once
#include <string>

struct Order {
  int userId_;
  int userOrderId_;
  std::string symbol_;
  int price_;
  int qty_;
  char side_;
};