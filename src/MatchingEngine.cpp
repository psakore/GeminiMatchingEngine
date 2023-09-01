// This file contain implementation of MatchingEngine class functions

#include "MatchingEngine.h"

namespace gemini {

void MatchingEngine::insertOrder(const OrderId &order_id, Side side, const Symbol &symbol, Quantity qty, Price price) {
  Order order{order_id, side, symbol, qty, price, ++m_ts};
  addOrder(std::move(order));
}

InsertError MatchingEngine::validateOrder(const Order &order) const {
  // Check for empty order id
  if (order.order_id.empty()) {
    return InsertError::InvalidOrderId;
  }

  // Check for valid side
  if (order.side != Side::Buy and order.side != Side::Sell) {
    return InsertError::InvalidSide;
  }

  // Check for empty symbol
  if (order.symbol.empty()) {
    return InsertError::InvalidSymbol;
  }

  // Check for positive price
  if (order.price <= 0) {
    return InsertError::InvalidPrice;
  }

  // Check for positive quantity
  if (order.qty <= 0) {
    return InsertError::InvalidQty;
  }

  return InsertError::OK;
}

void MatchingEngine::addOrder(Order &&order) {
  const auto err = validateOrder(order);
  // If order is not valid then call OnOrderInserted callback with appropriate InsertError
  if (err != InsertError::OK) {
    OnOrderInserted(order, err);
    return;
  }

  // Get the symbols orderbook
  OrderBook &order_book = m_symbol_to_orderbook[order.symbol];

  // Call tyeExecAndAddToBook with current order, orders side book and orders counter side book for checking trades
  if (order.side == Side::Buy) {
    tryExecAndAddToBook(std::move(order), order_book.bid_book, order_book.ask_book);
  } else {
    tryExecAndAddToBook(std::move(order), order_book.ask_book, order_book.bid_book);
  }
}

template <typename Book, typename CounterBook>
void MatchingEngine::tryExecAndAddToBook(Order &&order, Book &order_book, CounterBook &counter_order_book) {
  // Getting the comparator from order book, this will be used to compare price which determining trade conditions
  // For ask/sell side this should be std::less<Price> and for bid/buy side this should be std::greater<Price>
  const auto key_comp = counter_order_book.key_comp();

  // Iterating the counter side order book
  for (auto counter_price_lev_it = counter_order_book.begin();
       counter_price_lev_it != counter_order_book.end() and order.qty > 0;) {
    auto &[counter_price, counter_price_level] = *counter_price_lev_it;

    // If inserted orders price is less(ask/sell)/greater(bid/buy) then trade cannot be executed, break this loop and
    // add remaining order to orderbook
    if (key_comp(order.price, counter_price)) {
      break;
    }

    // Iterate the orders in this price level
    for (auto counter_order_it = counter_price_level.order_list.begin();
         counter_order_it != counter_price_level.order_list.end() and order.qty > 0;) {
      auto &counter_order = *counter_order_it;
      auto trade_qty = std::min(order.qty, counter_order.qty);

      counter_order.qty -= trade_qty;
      order.qty -= trade_qty;
      counter_price_level.total_qty -= trade_qty;

      // Call OnTrade callback with details of inserted order and counter order with traded qty and price
      OnTrade(order.symbol, order.order_id, counter_order.order_id, trade_qty, counter_price);

      if (counter_order.qty == 0) {
        counter_order_it = counter_price_level.order_list.erase(counter_order_it);
      }
    }

    if (counter_price_level.total_qty == 0) {
      counter_price_lev_it = counter_order_book.erase(counter_price_lev_it);
    }
  }

  // Call OnOrderInserted callback on inserted order with updated Quantity, if qty is 0 then the order is completely
  // filled
  OnOrderInserted(order, InsertError::OK);

  if (order.qty == 0) {
    return;
  }

  // Add the order to orderbook
  auto &price_level = order_book[order.price];
  price_level.total_qty += order.qty;
  price_level.order_list.push_back(std::move(order));
}

void MatchingEngine::getRemainingOrders() const {
  std::vector<Order> remaining_bid_orders, remaining_ask_orders;

  // Iterate all orderbooks and copy the orders from bid_book to bid_orders and ask_book to ask_orders
  for (const auto &[symbol, orderbook] : m_symbol_to_orderbook) {
    for (const auto &[price, price_level] : orderbook.ask_book) {
      std::copy(price_level.order_list.begin(), price_level.order_list.end(), std::back_inserter(remaining_ask_orders));
    }
    for (const auto &[price, price_level] : orderbook.bid_book) {
      std::copy(price_level.order_list.begin(), price_level.order_list.end(), std::back_inserter(remaining_bid_orders));
    }
  }

  // Comparator to compare orders on the basis of there arrival time
  static auto comp_order_by_ts = [](const Order &order1, const Order &order2) -> bool {
    return order1.timestamp < order2.timestamp;
  };

  // Sort remaining_bid_orders and remaining_ask_orders on the basis of arrival time
  std::sort(remaining_bid_orders.begin(), remaining_bid_orders.end(), comp_order_by_ts);
  std::sort(remaining_ask_orders.begin(), remaining_ask_orders.end(), comp_order_by_ts);

  // Merge the above two lists and send it to the OnGetRemainingOrders callback
  auto &remaining_orders = remaining_ask_orders;
  remaining_orders.reserve(remaining_bid_orders.size() + remaining_ask_orders.size());
  remaining_orders.insert(remaining_orders.end(), std::make_move_iterator(remaining_bid_orders.begin()),
                          std::make_move_iterator(remaining_bid_orders.end()));
  // Call OnRemainingOrders callback with remaining orders in the orderbook
  OnGetRemainingOrders(remaining_orders);
};

} // namespace gemini
