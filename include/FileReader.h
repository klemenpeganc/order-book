#pragma once
#include <fstream>
#include <functional>
#include <string_view>
class FileReader {
  using on_new_order_callback_t = std::function<void(int, const std::string &, int, int, char, int)>;
  using on_cancel_order_callback_t = std::function<void(int, int)>;
  using on_flush_orderbook_callback_t = std::function<void()>;

public:
  FileReader(std::string_view fileName, on_new_order_callback_t onNewOrderCallback,
             on_cancel_order_callback_t onCancelOrderCallback, on_flush_orderbook_callback_t onFlushOrderBookCallback);

private:
  std::ifstream fileStream_;
  on_new_order_callback_t onNewOrderInputCallback_;
  on_cancel_order_callback_t onCancelOrderInputCallback_;
  on_flush_orderbook_callback_t onFlushOrderBookCallback_;
};