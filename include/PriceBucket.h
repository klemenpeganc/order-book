#pragma once
#include "OrderBookEntry.h"

struct PriceBucket {
  OrderBookEntry *oldestOrder;
  OrderBookEntry *newestOrder;
};