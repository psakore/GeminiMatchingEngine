## Name: Prem Sakore 

### Instruction on how to build and run the application
cat sample_input.txt | ./run.sh
./run.sh build the docker and executes the matching_engine binary, This binary is present /app/MatchingEngine/install/bin/matching_engine in the docker 
This binary satisfies the Runtime Requirenments mentioned in the assignment i.e. Executing ./run.sh locally must build and launch a Docker image that defaults to running your application. The ./run.sh file must accept piped input.

### Instruction on how to build and run the tests
There is a ./test.sh present in the repo root. ./test.sh builds the docker. During this build a cmake flag is passed to build the tests as well. As tests required googletests, it downloads and builds the googletest repo. Then this runs ctest in the /app/MatchingEngine/\_build folder. This runs the gtest present (src/MatchingEngine.t.cpp). Tests can also be run from docker by running /app/MatchingEngine/install/bin/matching_engine_test binary manually.

### How I approached the problem?
- I have used cmake with c++17 for building this project and googletests to add and test unit tests
- I created a MatchingEngine class which has a symbol to order book map
- OrderBook contains the Buy side and Sell side orders of a particular symbol
- These individual OrderBooks are made up of PriceLevels which is a list of all order with the same price on Buy side and Sell side seperately
- As the Sell side and Buy side Order books are sorted by Price in increasing or decreasing manner, I have used std::map for it
- And to store list of orders at a particular price level, I have used std::list, as we are anyways using it in FIFO manner
- In this assignment we had to print all the remaining orders at the end, and they should be sorted by arrival time. As we dont need absolute time for tracking arrivalar, I have used a counter which increments for every new insert order thus keeping track of arrival time.
- Whenever an Order is inserted, first we try to fill the current order with orders in the counter/opposite side order book.
- After executing the order can be not filled at all or partially filled or completely filled. If it is completely filled then we dont need to add the order, in other case we can updated the remaining qty and add the order to the order book.
- I have used callbacks for Trade, Insertion and RemainingOrders, which have default callback which print the data in expected format. But these callbacks are user configurable and are currently updated in gtest to get these values on Trade, Insertion and RemainingOrders for comparing them with expected values instead of printing them.

### Time spent on this project
8 Hours

### Runtime Example

./run.sh example
```
$ cat sample_input.txt | ./run.sh
Sending build context to Docker daemon  4.603MB
Step 1/9 : FROM debian:11
 ---> 07585eb55737
Step 2/9 : RUN apt-get -y update   && apt-get -y install build-essential   && apt-get -y install cmake   && apt-get clean   && rm -rf /var/lib/apt/lists/*
 ---> Using cache
 ---> db235723fc35
Step 3/9 : WORKDIR /app/MatchingEngine
 ---> Using cache
 ---> ec9b0ad25448
Step 4/9 : COPY src src
 ---> Using cache
 ---> 99bc7b80c6fb
Step 5/9 : copy CMakeLists.txt .
 ---> Using cache
 ---> f91e614f0fb2
Step 6/9 : ARG TESTING
 ---> Using cache
 ---> cbbd3bc4df76
Step 7/9 : RUN echo "TESTING="$TESTING
 ---> Using cache
 ---> 65db71903ae8
Step 8/9 : RUN cmake -B _build -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -DBUILD_FOR_TESTING=$TESTING && cmake --build _build -- install -j4
 ---> Using cache
 ---> 996bda5fd807
Step 9/9 : RUN cd _build && ctest -VV
 ---> Using cache
 ---> 013974339156
Successfully built 013974339156
Successfully tagged gemini_matching_engine:latest
====== Match Engine =====
Enter 'exit' to quit
TRADE BTCUSD abe14 12345 5 10000
TRADE BTCUSD abe14 13471 2 9971
TRADE ETHUSD plu401 11431 5 175

zod42 SELL BTCUSD 2 10001
13471 BUY BTCUSD 4 9971
11431 BUY ETHUSD 4 175
45691 BUY ETHUSD 3 180
```

./test.sh example
```
$ ./test.sh
Sending build context to Docker daemon  4.627MB
Step 1/10 : FROM debian:11
 ---> 07585eb55737
Step 2/10 : RUN apt-get -y update   && apt-get -y install build-essential   && apt-get -y install cmake   && apt-get clean   && rm -rf /var/lib/apt/lists/*
 ---> Using cache
 ---> db235723fc35
Step 3/10 : WORKDIR /app/MatchingEngine
 ---> Using cache
 ---> ec9b0ad25448
Step 4/10 : COPY src src
 ---> Using cache
 ---> 99bc7b80c6fb
Step 5/10 : copy CMakeLists.txt .
 ---> Using cache
 ---> f91e614f0fb2
Step 6/10 : ARG TESTING
 ---> Using cache
 ---> cbbd3bc4df76
Step 7/10 : RUN echo "TESTING="$TESTING
 ---> Using cache
 ---> 756a2216686c
Step 8/10 : RUN cmake -B _build -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -DBUILD_FOR_TESTING=$TESTING && cmake --build _build -- install -j4
 ---> Using cache
 ---> b20eef670b89
Step 9/10 : ARG CACHEBUST
 ---> Using cache
 ---> e9aefb9b74cb
Step 10/10 : RUN echo ${CACHEBUST} && cd _build && ctest -VV
 ---> Running in bc28bf42ab94
1693526781
UpdateCTestConfiguration  from :/app/MatchingEngine/_build/DartConfiguration.tcl
UpdateCTestConfiguration  from :/app/MatchingEngine/_build/DartConfiguration.tcl
Test project /app/MatchingEngine/_build
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 0 tests to meet fixture requirements
Checking test dependency graph...
Checking test dependency graph end
test 1
      Start  1: MatchingEngineTest.addOrderInvalidOrderId

1: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addOrderInvalidOrderId" "--gtest_also_run_disabled_tests"
1: Test timeout computed to be: 10000000
1: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
1: Note: Google Test filter = MatchingEngineTest.addOrderInvalidOrderId
1: [==========] Running 1 test from 1 test suite.
1: [----------] Global test environment set-up.
1: [----------] 1 test from MatchingEngineTest
1: [ RUN      ] MatchingEngineTest.addOrderInvalidOrderId
1: [       OK ] MatchingEngineTest.addOrderInvalidOrderId (0 ms)
1: [----------] 1 test from MatchingEngineTest (0 ms total)
1: 
1: [----------] Global test environment tear-down
1: [==========] 1 test from 1 test suite ran. (0 ms total)
1: [  PASSED  ] 1 test.
 1/11 Test  #1: MatchingEngineTest.addOrderInvalidOrderId ...................   Passed    0.00 sec
test 2
      Start  2: MatchingEngineTest.addOrderInvalidSide

2: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addOrderInvalidSide" "--gtest_also_run_disabled_tests"
2: Test timeout computed to be: 10000000
2: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
2: Note: Google Test filter = MatchingEngineTest.addOrderInvalidSide
2: [==========] Running 1 test from 1 test suite.
2: [----------] Global test environment set-up.
2: [----------] 1 test from MatchingEngineTest
2: [ RUN      ] MatchingEngineTest.addOrderInvalidSide
2: [       OK ] MatchingEngineTest.addOrderInvalidSide (0 ms)
2: [----------] 1 test from MatchingEngineTest (0 ms total)
2: 
2: [----------] Global test environment tear-down
2: [==========] 1 test from 1 test suite ran. (0 ms total)
2: [  PASSED  ] 1 test.
 2/11 Test  #2: MatchingEngineTest.addOrderInvalidSide ......................   Passed    0.00 sec
test 3
      Start  3: MatchingEngineTest.addOrderInvalidSymbol

3: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addOrderInvalidSymbol" "--gtest_also_run_disabled_tests"
3: Test timeout computed to be: 10000000
3: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
3: Note: Google Test filter = MatchingEngineTest.addOrderInvalidSymbol
3: [==========] Running 1 test from 1 test suite.
3: [----------] Global test environment set-up.
3: [----------] 1 test from MatchingEngineTest
3: [ RUN      ] MatchingEngineTest.addOrderInvalidSymbol
3: [       OK ] MatchingEngineTest.addOrderInvalidSymbol (0 ms)
3: [----------] 1 test from MatchingEngineTest (0 ms total)
3: 
3: [----------] Global test environment tear-down
3: [==========] 1 test from 1 test suite ran. (0 ms total)
3: [  PASSED  ] 1 test.
 3/11 Test  #3: MatchingEngineTest.addOrderInvalidSymbol ....................   Passed    0.00 sec
test 4
      Start  4: MatchingEngineTest.addOrderInvalidQty

4: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addOrderInvalidQty" "--gtest_also_run_disabled_tests"
4: Test timeout computed to be: 10000000
4: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
4: Note: Google Test filter = MatchingEngineTest.addOrderInvalidQty
4: [==========] Running 1 test from 1 test suite.
4: [----------] Global test environment set-up.
4: [----------] 1 test from MatchingEngineTest
4: [ RUN      ] MatchingEngineTest.addOrderInvalidQty
4: [       OK ] MatchingEngineTest.addOrderInvalidQty (0 ms)
4: [----------] 1 test from MatchingEngineTest (0 ms total)
4: 
4: [----------] Global test environment tear-down
4: [==========] 1 test from 1 test suite ran. (0 ms total)
4: [  PASSED  ] 1 test.
 4/11 Test  #4: MatchingEngineTest.addOrderInvalidQty .......................   Passed    0.00 sec
test 5
      Start  5: MatchingEngineTest.addOrderInvalidPrice

5: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addOrderInvalidPrice" "--gtest_also_run_disabled_tests"
5: Test timeout computed to be: 10000000
5: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
5: Note: Google Test filter = MatchingEngineTest.addOrderInvalidPrice
5: [==========] Running 1 test from 1 test suite.
5: [----------] Global test environment set-up.
5: [----------] 1 test from MatchingEngineTest
5: [ RUN      ] MatchingEngineTest.addOrderInvalidPrice
5: [       OK ] MatchingEngineTest.addOrderInvalidPrice (0 ms)
5: [----------] 1 test from MatchingEngineTest (0 ms total)
5: 
5: [----------] Global test environment tear-down
5: [==========] 1 test from 1 test suite ran. (0 ms total)
5: [  PASSED  ] 1 test.
 5/11 Test  #5: MatchingEngineTest.addOrderInvalidPrice .....................   Passed    0.00 sec
test 6
      Start  6: MatchingEngineTest.addValidOrder

6: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.addValidOrder" "--gtest_also_run_disabled_tests"
6: Test timeout computed to be: 10000000
6: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
6: Note: Google Test filter = MatchingEngineTest.addValidOrder
6: [==========] Running 1 test from 1 test suite.
6: [----------] Global test environment set-up.
6: [----------] 1 test from MatchingEngineTest
6: [ RUN      ] MatchingEngineTest.addValidOrder
6: [       OK ] MatchingEngineTest.addValidOrder (0 ms)
6: [----------] 1 test from MatchingEngineTest (0 ms total)
6: 
6: [----------] Global test environment tear-down
6: [==========] 1 test from 1 test suite ran. (0 ms total)
6: [  PASSED  ] 1 test.
 6/11 Test  #6: MatchingEngineTest.addValidOrder ............................   Passed    0.00 sec
test 7
      Start  7: MatchingEngineTest.checkNumberOfTrades

7: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.checkNumberOfTrades" "--gtest_also_run_disabled_tests"
7: Test timeout computed to be: 10000000
7: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
7: Note: Google Test filter = MatchingEngineTest.checkNumberOfTrades
7: [==========] Running 1 test from 1 test suite.
7: [----------] Global test environment set-up.
7: [----------] 1 test from MatchingEngineTest
7: [ RUN      ] MatchingEngineTest.checkNumberOfTrades
7: [       OK ] MatchingEngineTest.checkNumberOfTrades (0 ms)
7: [----------] 1 test from MatchingEngineTest (0 ms total)
7: 
7: [----------] Global test environment tear-down
7: [==========] 1 test from 1 test suite ran. (0 ms total)
7: [  PASSED  ] 1 test.
 7/11 Test  #7: MatchingEngineTest.checkNumberOfTrades ......................   Passed    0.00 sec
test 8
      Start  8: MatchingEngineTest.validateTrades

8: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.validateTrades" "--gtest_also_run_disabled_tests"
8: Test timeout computed to be: 10000000
8: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
8: Note: Google Test filter = MatchingEngineTest.validateTrades
8: [==========] Running 1 test from 1 test suite.
8: [----------] Global test environment set-up.
8: [----------] 1 test from MatchingEngineTest
8: [ RUN      ] MatchingEngineTest.validateTrades
8: [       OK ] MatchingEngineTest.validateTrades (0 ms)
8: [----------] 1 test from MatchingEngineTest (0 ms total)
8: 
8: [----------] Global test environment tear-down
8: [==========] 1 test from 1 test suite ran. (0 ms total)
8: [  PASSED  ] 1 test.
 8/11 Test  #8: MatchingEngineTest.validateTrades ...........................   Passed    0.00 sec
test 9
      Start  9: MatchingEngineTest.validateRemainingOrders

9: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.validateRemainingOrders" "--gtest_also_run_disabled_tests"
9: Test timeout computed to be: 10000000
9: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
9: Note: Google Test filter = MatchingEngineTest.validateRemainingOrders
9: [==========] Running 1 test from 1 test suite.
9: [----------] Global test environment set-up.
9: [----------] 1 test from MatchingEngineTest
9: [ RUN      ] MatchingEngineTest.validateRemainingOrders
9: [       OK ] MatchingEngineTest.validateRemainingOrders (0 ms)
9: [----------] 1 test from MatchingEngineTest (0 ms total)
9: 
9: [----------] Global test environment tear-down
9: [==========] 1 test from 1 test suite ran. (0 ms total)
9: [  PASSED  ] 1 test.
 9/11 Test  #9: MatchingEngineTest.validateRemainingOrders ..................   Passed    0.00 sec
test 10
      Start 10: MatchingEngineTest.validateTradesAndRemainingOrders

10: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.validateTradesAndRemainingOrders" "--gtest_also_run_disabled_tests"
10: Test timeout computed to be: 10000000
10: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
10: Note: Google Test filter = MatchingEngineTest.validateTradesAndRemainingOrders
10: [==========] Running 1 test from 1 test suite.
10: [----------] Global test environment set-up.
10: [----------] 1 test from MatchingEngineTest
10: [ RUN      ] MatchingEngineTest.validateTradesAndRemainingOrders
10: [       OK ] MatchingEngineTest.validateTradesAndRemainingOrders (0 ms)
10: [----------] 1 test from MatchingEngineTest (0 ms total)
10: 
10: [----------] Global test environment tear-down
10: [==========] 1 test from 1 test suite ran. (0 ms total)
10: [  PASSED  ] 1 test.
10/11 Test #10: MatchingEngineTest.validateTradesAndRemainingOrders .........   Passed    0.00 sec
test 11
      Start 11: MatchingEngineTest.validateTradesMultipleOrderInSameLevel

11: Test command: /app/MatchingEngine/_build/src/matching_engine_test "--gtest_filter=MatchingEngineTest.validateTradesMultipleOrderInSameLevel" "--gtest_also_run_disabled_tests"
11: Test timeout computed to be: 10000000
11: Running main() from /app/MatchingEngine/_build/_deps/googletest-src/googletest/src/gtest_main.cc
11: Note: Google Test filter = MatchingEngineTest.validateTradesMultipleOrderInSameLevel
11: [==========] Running 1 test from 1 test suite.
11: [----------] Global test environment set-up.
11: [----------] 1 test from MatchingEngineTest
11: [ RUN      ] MatchingEngineTest.validateTradesMultipleOrderInSameLevel
11: [       OK ] MatchingEngineTest.validateTradesMultipleOrderInSameLevel (0 ms)
11: [----------] 1 test from MatchingEngineTest (0 ms total)
11: 
11: [----------] Global test environment tear-down
11: [==========] 1 test from 1 test suite ran. (0 ms total)
11: [  PASSED  ] 1 test.
11/11 Test #11: MatchingEngineTest.validateTradesMultipleOrderInSameLevel ...   Passed    0.00 sec

100% tests passed, 0 tests failed out of 11

Total Test time (real) =   0.06 sec
Removing intermediate container bc28bf42ab94
 ---> 98fd1bfb0e44
Successfully built 98fd1bfb0e44
Successfully tagged gemini_matching_engine:latest

```
