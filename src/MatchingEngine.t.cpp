#include <vector>

#include "MatchingEngine.h"
#include "gtest/gtest.h"

namespace gemini {

// Structure to hold and compare Trades
struct TradeT {
  Symbol symbol;
  OrderId order_id;
  OrderId counter_order_id;
  Quantity qty;
  Price price;
};

// Structure to hold and compare Orders
struct OrderT : Order {
  InsertError status;
};

bool operator==(const Order &a, const Order &b) {
  return a.symbol == b.symbol and a.order_id == b.order_id and a.side == b.side and a.qty == b.qty and
         a.price == b.price;
}

bool operator==(const TradeT &a, const TradeT &b) {
  return a.symbol == b.symbol and a.order_id == b.order_id and a.counter_order_id == b.counter_order_id and
         a.qty == b.qty and a.price == b.price;
}

std::ostream &operator<<(std::ostream &os, const TradeT &t) {
  os << t.symbol << " " << t.order_id << " " << t.counter_order_id << " " << t.qty << " " << t.price;
  return os;
}

bool operator==(const OrderT &a, const OrderT &b) {
  return a.order_id == b.order_id and a.side == b.side and a.symbol == b.symbol and a.qty == b.qty and
         a.price == b.price;
}

// Test Fixture class for MatchingEngine
class MatchingEngineTest : public ::testing::Test {
protected:
  // Sets the callbacks to lamda functions which store the values passed to the callbacks in std::vectors
  void SetUp() override {
    m_matching_engine.OnOrderInserted = [this](const Order &order, InsertError err) -> void {
      m_inserted_orders.push_back(OrderT{order, err});
    };

    m_matching_engine.OnTrade = [this](const Symbol &symbol, const OrderId &order_id, const OrderId &counter_order_id,
                                       Quantity qty, Price price) -> void {
      m_trades.push_back(TradeT{symbol, order_id, counter_order_id, qty, price});
    };

    m_matching_engine.OnGetRemainingOrders = [this](const std::vector<Order> remaining_orders) {
      m_remaining_orders = remaining_orders;
    };
  }

  // InsertOrder which takes the input as a string, same way like main.cpp
  void insertOrder(const std::string &line, InsertError expected_status, int line_number) {
    OrderId order_id;
    Side side;
    Symbol symbol;
    Quantity qty;
    Price prc;
    MatchingEngine matching_engine;

    std::stringstream ss(line);
    ss >> order_id >> side >> symbol >> qty >> prc;

    insertOrder(order_id, side, symbol, qty, prc, expected_status, line_number);
  }

  // Insert Order which takes order info and expected status after calling the insertOrder for MatchingEngine
  void insertOrder(OrderId order_id, Side side, Symbol symbol, Quantity qty, Price prc, InsertError expected_status,
                   int line_number) {
    auto o_size = m_inserted_orders.size();
    m_matching_engine.insertOrder(order_id, side, symbol, qty, prc);

    // 1 entry should be added to m_inserted_orders
    ASSERT_EQ(m_inserted_orders.size(), o_size + 1) << "Failed at line: " + std::to_string(line_number);
    // Expect the status to be equal to the passed expected_status
    EXPECT_EQ(m_inserted_orders.back().status, expected_status) << "Failed at line: " + std::to_string(line_number);
  }

  auto getTrades() {
    auto trades = m_trades;
    m_trades.clear();
    return trades;
  }

  auto getInsertedOrders() {
    auto inserted_orders = m_inserted_orders;
    m_inserted_orders.clear();
    return inserted_orders;
  }

  auto getRemainingOrders() {
    m_matching_engine.getRemainingOrders();
    auto remaining_orders = m_remaining_orders;
    m_remaining_orders.clear();
    return remaining_orders;
  }

  std::vector<TradeT> m_trades;
  std::vector<OrderT> m_inserted_orders;
  std::vector<Order> m_remaining_orders;
  gemini::MatchingEngine m_matching_engine;
};

TEST_F(MatchingEngineTest, addOrderInvalidOrderId) {
  // Empty Order ID
  insertOrder("", Side::Buy, "BTCUSD", 2, 10000, InsertError::InvalidOrderId, __LINE__);
}

TEST_F(MatchingEngineTest, addOrderInvalidSide) {
  // Invalid Side BU
  insertOrder("12345 BU BTCUSD 5 10000", InsertError::InvalidSide, __LINE__);

  auto inserted_orders = getInsertedOrders();
  ASSERT_EQ(inserted_orders.size(), 1);
  EXPECT_EQ(inserted_orders[0].side, Side::Invalid);

  // Invalid Side
  insertOrder("12345", Side::Invalid, "BTCUSD", 2, 10000, InsertError::InvalidSide, __LINE__);

  inserted_orders = getInsertedOrders();
  ASSERT_EQ(inserted_orders.size(), 1);
  EXPECT_EQ(inserted_orders[0].side, Side::Invalid);
}

TEST_F(MatchingEngineTest, addOrderInvalidSymbol) {
  // Empty Symbol
  insertOrder("12345", Side::Buy, "", 2, 10000, InsertError::InvalidSymbol, __LINE__);
}

TEST_F(MatchingEngineTest, addOrderInvalidQty) {
  // 0 Qty
  insertOrder("12345 BUY BTCUSD 0 10000", InsertError::InvalidQty, __LINE__);
  // Negative Qty
  insertOrder("12345 BUY BTCUSD -5 10000", InsertError::InvalidQty, __LINE__);
  // Non Integer Qty
  insertOrder("12345 BUY BTCUSD a 10000", InsertError::InvalidQty, __LINE__);
}

TEST_F(MatchingEngineTest, addOrderInvalidPrice) {
  // 0 Price
  insertOrder("12345 BUY BTCUSD 5 0", InsertError::InvalidPrice, __LINE__);
  // Negative Price
  insertOrder("12345 BUY BTCUSD 4 -100", InsertError::InvalidPrice, __LINE__);
  // Non Integer Price
  insertOrder("12345 BUY BTCUSD 5 a", InsertError::InvalidPrice, __LINE__);
}

TEST_F(MatchingEngineTest, addValidOrder) {
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);

  auto inserted_orders = getInsertedOrders();
  ASSERT_EQ(inserted_orders.size(), 3);
  ASSERT_EQ(inserted_orders[0], (OrderT{"12345", Side::Buy, "BTCUSD", 5, 10000}));
  ASSERT_EQ(inserted_orders[1], (OrderT{"zod42", Side::Sell, "BTCUSD", 2, 10001}));
  ASSERT_EQ(inserted_orders[2], (OrderT{"13471", Side::Buy, "BTCUSD", 6, 9971}));
}

TEST_F(MatchingEngineTest, checkNumberOfTrades) {
  // Should not trade
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  auto trades = getTrades();
  ASSERT_TRUE(trades.empty());

  // Should not trade
  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  // Should not trade
  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  // Fills two orders
  insertOrder("abe14 SELL BTCUSD 7 9800", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_EQ(trades.size(), 2);

  auto inserted_orders = getInsertedOrders();
  ASSERT_EQ(inserted_orders.size(), 4);
  ASSERT_EQ(inserted_orders[0], (OrderT{"12345", Side::Buy, "BTCUSD", 5, 10000}));

  ASSERT_EQ(inserted_orders[1], (OrderT{"zod42", Side::Sell, "BTCUSD", 2, 10001}));
  ASSERT_EQ(inserted_orders[2], (OrderT{"13471", Side::Buy, "BTCUSD", 6, 9971}));

  // Updated Quantity (from 7 to 0) in Order
  ASSERT_EQ(inserted_orders[3], (OrderT{"abe14", Side::Sell, "BTCUSD", 0, 9800}));
}

TEST_F(MatchingEngineTest, validateTrades) {
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  auto trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("abe14 SELL BTCUSD 7 9800", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_EQ(trades.size(), 2);

  // Validating order_id, counter_order_id, trade qty and price
  ASSERT_EQ(trades[0], (TradeT{"BTCUSD", "abe14", "12345", 5, 10000}));
  ASSERT_EQ(trades[1], (TradeT{"BTCUSD", "abe14", "13471", 2, 9971}));
}

TEST_F(MatchingEngineTest, validateRemainingOrders) {
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  auto trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("abe14 SELL BTCUSD 7 9800", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_EQ(trades.size(), 2);

  auto remaining_orders = getRemainingOrders();
  // 2 Orders should remain in the books
  ASSERT_EQ(remaining_orders.size(), 2);

  EXPECT_EQ(remaining_orders[0], (Order{"zod42", Side::Sell, "BTCUSD", 2, 10001}));
  // Qty is updated from 6 to 4 ad 2 Qty is partially filled
  EXPECT_EQ(remaining_orders[1], (Order{"13471", Side::Buy, "BTCUSD", 4, 9971}));
}

TEST_F(MatchingEngineTest, validateTradesAndRemainingOrders) {
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  auto trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("11431 BUY ETHUSD 9 175", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("abe14 SELL BTCUSD 7 9800", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_EQ(trades.size(), 2);

  ASSERT_EQ(trades[0], (TradeT{"BTCUSD", "abe14", "12345", 5, 10000}));
  ASSERT_EQ(trades[1], (TradeT{"BTCUSD", "abe14", "13471", 2, 9971}));

  insertOrder("plu401 SELL ETHUSD 5 170", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_EQ(trades[0], (TradeT{"ETHUSD", "plu401", "11431", 5, 175}));

  insertOrder("45691 BUY ETHUSD 3 180", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  auto remaining_orders = getRemainingOrders();
  ASSERT_EQ(remaining_orders.size(), 4);

  EXPECT_EQ(remaining_orders[0], (Order{"zod42", Side::Sell, "BTCUSD", 2, 10001}));
  EXPECT_EQ(remaining_orders[1], (Order{"13471", Side::Buy, "BTCUSD", 4, 9971}));
  EXPECT_EQ(remaining_orders[2], (Order{"11431", Side::Buy, "ETHUSD", 4, 175}));
  EXPECT_EQ(remaining_orders[3], (Order{"45691", Side::Buy, "ETHUSD", 3, 180}));
}

TEST_F(MatchingEngineTest, validateTradesMultipleOrderInSameLevel) {
  insertOrder("12345 BUY BTCUSD 5 10000", InsertError::OK, __LINE__);
  auto trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("zod42 SELL BTCUSD 2 10001", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13471 BUY BTCUSD 6 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13472 BUY BTCUSD 10 9971", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("13473 BUY BTCUSD 4 9972", InsertError::OK, __LINE__);
  trades = getTrades();
  ASSERT_TRUE(trades.empty());

  insertOrder("abe14 SELL BTCUSD 20 9800", InsertError::OK, __LINE__);
  trades = getTrades();
  EXPECT_EQ(trades.size(), 4);

  EXPECT_EQ(trades[0], (TradeT{"BTCUSD", "abe14", "12345", 5, 10000}));
  EXPECT_EQ(trades[1], (TradeT{"BTCUSD", "abe14", "13473", 4, 9972}));
  EXPECT_EQ(trades[2], (TradeT{"BTCUSD", "abe14", "13471", 6, 9971}));
  EXPECT_EQ(trades[3], (TradeT{"BTCUSD", "abe14", "13472", 5, 9971}));

  auto remaining_orders = getRemainingOrders();
  ASSERT_EQ(remaining_orders.size(), 2);
  EXPECT_EQ(remaining_orders[0], (Order{"zod42", Side::Sell, "BTCUSD", 2, 10001}));
  EXPECT_EQ(remaining_orders[1], (Order{"13472", Side::Buy, "BTCUSD", 5, 9971}));
}

} // namespace gemini
