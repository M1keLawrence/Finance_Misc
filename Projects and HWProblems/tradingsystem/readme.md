========================================================================
========================================================================
How to run project: 

========================================================================
========================================================================
// In tradingsystem directory
// Make files
Touch build
Open build
Cmake ..
Make 

========================================================================
========================================================================

// Now run executables. Order (1,2,3,4) -> 5 -> (6,7,8)

// Creates data; number is how much data to create per asset.
Terminal 1: ./gen_data 1000000

// Connects to execution outbound socket and prints received execs.
// 
Terminal 2: ./exec_print 

// Connects to streaming outbound socket and prints received streams.
// Will have action (many streams) once prices are published via terminal 6. 
Terminal 3: ./stream_print

// Publishes MarketData.txt into shared memory ring buffer
Terminal 4: ./md_shm_publisher

// Runs all services and connectors, listens on ports, writes outputs. 
Terminal 5: ./trading_system

// Reads prices.txt and publishes each line to the pricing socket 
Terminal 6: ./prices_publisher prices.txt 127.0.0.1 9001

// Reads trades.txt and publishes each line to the trades socket 
Terminal 7: ./trades_publisher trades.txt 127.0.0.1 9002

// Reads inquiries.txt and publishes each line to the inquiries socket
Terminal 8: ./inquiries_publisher inquiries.txt 127.0.0.1 9003 

========================================================================
========================================================================

Notes: 

Used Shared Memory for the market data, used TCP connectors for price, trades, and inquiries data. 

./md_shm_publisher must run BEFORE ./trading_system, otherwise the later will pop a bad data read from the shared memory before it should, which short circuits something and prevents the rest from being read. The former must connect first and wipe clean the shared memory location, then read in fresh data. 

Project originally had further issues with using shared memory, as trading_system would read leftover data in buffer and throw all subsequent data reads off. Patched this by 


========================================================================
========================================================================

Runtime: 

./gen_data 1,000,000 takes ~ 1:09 min to generate the data

The rest can be done concurrently and takes ~6:10min (streams) and ~7:10min (exec), for a total runtime of ~7:20min. 