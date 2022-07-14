#include "FileWriter.h"

FileWriter::FileWriter(std::string_view filename) : fileStream_(filename) {}

void FileWriter::writeOrderAcknowledgment(int userId, int userOrderId) { fileStream_ << "A, " << userId << ", " << userOrderId << '\n'; }
void FileWriter::writeTradeAcknowledgment(int userIdBuy, int userOrderIdBuy, int userIdSell, int userOrderIdSell, unsigned int price,
                                          int quantity) {
  fileStream_ << "T, " << userIdBuy << ", " << userOrderIdBuy << ", " << userIdSell << ", " << userOrderIdSell << ", " << price << ", "
              << quantity << '\n';
}
void FileWriter::writeBboChange(char side, unsigned int price, int quantity) {
  fileStream_ << "B, " << side << ", " << price << ", " << quantity << '\n';
}