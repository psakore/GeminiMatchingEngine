#include <iostream>
#include <sstream>
#include <stdio.h>

#include "MatchingEngine.h"

int main() {
  std::cerr << "====== Match Engine =====" << std::endl;
  std::cerr << "Enter 'exit' to quit" << std::endl;
  std::string line;

  gemini::OrderId order_id;
  gemini::Side side; // istream is overloaded for gemini::Side
  gemini::Symbol symbol;
  gemini::Quantity qty;
  gemini::Price prc;
  gemini::MatchingEngine matching_engine;

  while (getline(std::cin, line) && line != "exit") {
    std::stringstream ss(line);
    ss >> order_id >> side >> symbol >> qty >> prc;
    matching_engine.insertOrder(order_id, side, symbol, qty, prc);
  }

  matching_engine.getRemainingOrders();
  return 0;
}
