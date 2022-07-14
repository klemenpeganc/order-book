#pragma once
#include <fstream>
#include <string_view>
class FileWriter {

public:
  FileWriter(std::string_view fileName);

  void writeOrderAcknowledgment(int userId, int userOrderId);
  void writeTradeAcknowledgment(int userIdBuy, int userOrderIdBuy, int userIdSell, int userOrderIdSell, unsigned int price, int quantity);
  void writeBboChange(char side, unsigned int price, int quantity);

private:
  std::ofstream fileStream_;
};