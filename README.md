# Building
  - type make.  This will compile assetDb and run a test with a small ( 100 asset) database
  - when building it runs the test using run_test.py, here is the syntax

./run_test.py --help
usage: run_test.py [-h] [-assets ASSETS] [-skus SKUS] [-stationary STATIONARY]
                   [-regions REGIONS]

assetDb query profile test tool

optional arguments:
  -h, --help            show this help message and exit
  -assets ASSETS        Total number of assets to generate
  -skus SKUS            Total number of SKUs
  -stationary STATIONARY
                        0-100, percentage of stationary assets
  -regions REGIONS      Total number of regions to generate


example: ./run_test.py -assets 100000 -skus 2000 -regions 500 -stationary .9

# Assumptions
  - Asset tracking is global with global coordinates
  - A small percentage of assets are not stationary
  - Queries won't happen for assets that are not in the database
  - The speed of a query is more important than the memory footprint of the database
  - Retrieval system is single threaded but could scale with threads
  - A user is interested in stationary assets to either remove from the database or place in transit
  - A user is interested in non-stationary assets to predict future inventory levels
  - A user will want to query which assets are in a certain area of the world defined by 2 longitude coordinates and 2 latitude coordinates
  - The number of SKUs is much smaller than the total number of assets 10's of thousands verses millions or billions. 
  - IP addresses are statically assigned, fully random, and have no relationship with proximity of assets to each other
- # Design
  - The database consists of 3 nested key:value hash tables that organized by SKU,coordinates, then IP address. Example: assetDb[SKU][Coordinates][IP].  The Coordinates key is the longitude and latitude concatenated together to avoid a hash lookup for both the latitude and longitude.
  - One hash table represents stationary assets, the other represents non-stationary assets.
  - The Longitude and Latitude keys are rounded to the nearest integer representing a search space by the upper left corner Longitude/Latitude intersection
  - To look up a specific IP address, all of the [SKU][Coordinates] hash tables get checked linearly for the existence of the IP.  If the user knows the SKU information, that will speed up the search by 1 hash lookup. If they know the coordinates, that will make it even faster.
  - the AssetDb class will have the following methods
    - constructor : loads the csv file created by the Python scripts
    - query - 
      - All assets in a SKU.
        - query ( string SKU, unsigned int StationaryMask)
      - All assets in a SKU in a region of the world containing these coordinates
        - query ( string SKU , float latitude, float longitude,unsigned int StationaryMask)
      - All assets in a region.
        - query ( float latitude, float longitude, unsigned int StationaryMask)
      - Asset with a specific IP address.
        - query ( string IP )
      - List all SKUs
      - List all regions
- # Results - measured on a basic laptop
## Database with 100,000 assets
    ./run_test.py -assets 100000 -skus 10000 -regions 2000
    Creating an assetDb.csv file with 100000 assets,  10000  SKUs,  2000  regions
        90 % of assets will be stationary
    PERF: Average time to query 10000 SKUs 6 microseconds
    PERF: Average time to query 1961 Regions 83 microseconds
    PERF: Average time to query 100 IP addresses 0.84 microseconds
## Database with 1,000,000 assets
    ./run_test.py -assets 1000000 -skus 10000 -regions 2000
    Creating an assetDb.csv file with 1000000 assets,  10000  SKUs,  2000  regions
        90 % of assets will be stationary
    PERF: Average time to query 10000 SKUs 64 microseconds
    PERF: Average time to query 1966 Regions 9 microseconds
    PERF: Average time to query 100 IP addresses 1.08 microseconds
- # Improvements to make
  - Break the progcode ram into header/code files as opposed to a single cpp file and add more comments.
  - Use a standard database like postgress/mySQL
  - Performance : encode a unique sub-node index and SKU index into the lower 64-bits of the IPv6 address.  This would reduce the time to generate a hash, reduce collissions, and eliminate the need for nested tables.  The cost would for when the SKU or node would change for a given transmitter we would need to also make a change to the IP address.
  - Thread the IP address search to look through all of the SKUs in parallel.
  - All Latitude/Longitude boxes are not equal in area Which will create larger search areas near the equator.  Dividing the areas closer to the equator within degres would be better for more accurate tracking near the equator.
  - There is an unordered map in the boost library but I didn't find a speific benefit in my research
  - Give the database query function more powerful regular expressions using the boost library
  - Pass queried records by reference rather than copy into a vector
  - More testing on all of the functions
  - Create another fast lookup layout where the Region is the first key and then the SKU to speed up regional retrievals
  - add a more generic interface that looks more like a database query
