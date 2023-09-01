#pragma once

#include "Types.h"
#include "Utils.hpp"

namespace gemini {

class MatchingEngine {
public:
  // Inserts an order in the Matching Engine, this can lead to trades and order getting added to order book
  // On occurence of trade it calls OnTrade callback
  // For every order OnOrderInserted callback is called with appropriate InsertError set in it
  void insertOrder(const OrderId &order_id, Side side, const Symbol &symbol, Quantity qty, Price price);

  // Return the remaing orders from the matching engine to the OnGetRemainingOrders callback
  void getRemainingOrders() const;

  // OnTrade callback, this is called for every occurence of Trade
  // DefaultOnTradeFunc prints the Trade to STDOUT
  OrderTradedFunction OnTrade = DefaultOnTradeFunc;

  // OnOrderInserted callback, this is called for every order passed to matchingEngine for insertion
  // DefaultOnGetRemainingOrdersFunc does nothing
  OrderInsertedFunction OnOrderInserted = DefaultOnOrderInsertedFunc;

  // OnGetRemainingOrders callback, called with vector of remaining orders in MatchingEngine after getRemainingOrders is
  // called DefaultOnGetRemaminigOrdersFunc prints the remaining orders to STDOUT
  GetRemainingOrderFunction OnGetRemainingOrders = DefaultOnGetRemainingOrdersFunc;

private:
  // Helper function to validate a order before trying to trade or insert
  InsertError validateOrder(const Order &order) const;

  // Helper function to add order in the order book
  void addOrder(Order &&order);

  // This functions first tries to execute trades possible for the current order
  // After trades executed, if the order still has some or all qty left, it adds the orders to the OrderBook
  template <typename Book, typename CounterBook>
  void tryExecAndAddToBook(Order &&order, Book &book, CounterBook &counter_order_book);

private:
  TimeStamp m_ts{0};                          // Pseudo time tracking variable to track which order arrived first
  SymbolToOrderBookMap m_symbol_to_orderbook; // Symbol to OrderBook Map
};

} // namespace gemini
