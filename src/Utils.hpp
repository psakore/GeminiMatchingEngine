#pragma once

// This file contains the helper functions for cin, cout and Default callbacks
#include "Types.h"

#include <iostream>

namespace gemini {

inline std::ostream &operator<<(std::ostream &os, Side side) {
  switch (side) {
  case Side::Buy:
    os << "BUY";
    break;
  case Side::Sell:
    os << "SELL";
    break;
  default:
    os << "Invalid";
    break;
  }
  return os;
}

inline std::istream &operator>>(std::istream &is, Side &side) {
  std::string side_str;
  is >> side_str;
  if (side_str == "BUY") {
    side = Side::Buy;
  } else if (side_str == "SELL") {
    side = Side::Sell;
  } else {
    side = Side::Invalid;
  }
  return is;
}

inline std::ostream &operator<<(std::ostream &os, InsertError err) {
  switch (err) {
  case InsertError::OK:
    os << "OK";
    break;
  case InsertError::SymbolNotFound:
    os << "SymbolNotFound";
    break;
  case InsertError::InvalidOrderId:
    os << "InvalidOrderId";
    break;
  case InsertError::InvalidSymbol:
    os << "InvalidSymbol";
    break;
  case InsertError::InvalidSide:
    os << "InvalidSide";
    break;
  case InsertError::InvalidPrice:
    os << "InvalidPrice";
    break;
  case InsertError::InvalidQty:
    os << "InvalidQty";
    break;
  case InsertError::SystemError:
    os << "SystemError";
    break;
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Order &order) {
  os << order.order_id << " " << order.side << " " << order.symbol << " " << order.qty << " " << order.price;
  return os;
}

static void DefaultOnTradeFunc(const Symbol &symbol, const OrderId &order_id, const OrderId &counter_order_id,
                               Quantity qty, Price price) {
  std::cout << "TRADE " << symbol << " " << order_id << " " << counter_order_id << " " << qty << " " << price
            << std::endl;
}

static void DefaultOnOrderInsertedFunc(const Order &order, InsertError err) {
  return;
  // std::cerr << "ORDER: " << order << " STATUS: " << err << std::endl;
}

static void DefaultOnGetRemainingOrdersFunc(const std::vector<Order> &orders) {
  std::cout << std::endl;
  for (const auto &order : orders) {
    std::cout << order << std::endl;
  }
}
} // namespace gemini
