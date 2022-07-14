# Order book Kraken

### Utility classes
- Bbo: describes the top of the book
- Order: describes the incoming order
- Trade: describes the executed trade
- PriceBucket: describes the orders at a specific price level
- OrderBookEntry: describes the order entry and points to the next order in the price bucket
- FileReader: reads in the input data and issues a callback for each order book action needed
- FileWriter: outputs the orders, trades and bbo updates to a CSV file.

### Order book functionality
Apart from comments in the code:
- we use a pairing function to put the orders in the order book, which can cause troubles as the distance between order entries can become quite
big.


### Improvements needed
- Bbo updates are not fully correct, if the order is filled, the bbo doesnt update to the second best price and quantity.
- use alternative to pairing function, as that results in need of reserving a bigger chunk of memory that we need.
- introduce threading for filewriter, filereader and each orderbook
- support multiple orderbooks, as currently it only supports one symbol.

### Build instructions

Requirements:
- CMake

To Run:
`./build_all.sh`
`./run.sh`