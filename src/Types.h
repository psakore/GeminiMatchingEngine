#pragma once
// This File contains all the Structures and DataTypes required for MatchingEngine

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <string>

namespace gemini {
using Symbol = std::string;
using OrderId = std::string;
using Quantity = int32_t;
using Price = int32_t;
using TimeStamp = uint64_t;

enum class Side { Invalid, Buy, Sell };

struct Order {
  OrderId order_id;
  Side side;
  Symbol symbol;
  Quantity qty;
  Price price;
  TimeStamp timestamp;
};

struct PriceLevel {
  Quantity total_qty{0};
  std::list<Order> order_list;
};

struct OrderBook {
  std::map<Price, PriceLevel> ask_book;
  std::map<Price, PriceLevel, std::greater<Price>> bid_book;
};

using SymbolToOrderBookMap = std::unordered_map<Symbol, OrderBook>;

enum class InsertError {
  OK,
  SymbolNotFound,
  InvalidOrderId,
  InvalidSymbol,
  InvalidSide,
  InvalidPrice,
  InvalidQty,
  SystemError,
};

// Function Signatures for MatchingEngine class callbacks
using OrderInsertedFunction = std::function<void(const Order &order, InsertError err)>;

using OrderTradedFunction = std::function<void(const Symbol &symbol, const OrderId &order_id,
                                               const OrderId &counter_order_id, Quantity qty, Price price)>;

using GetRemainingOrderFunction = std::function<void(const std::vector<Order> &orders)>;
} // namespace gemini
