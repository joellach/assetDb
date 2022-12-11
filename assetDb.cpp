#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <chrono>
using namespace std::chrono;
class assetDbManager {
    public:
        typedef struct AssetCoordinates {
            float Longitude;
            float Latitude;
            int Stationary;
            std::string SKU;
            std::string IP;
        } ASSET_COORDINATES;
        enum Stationary {STATIONARY, NOT_STATIONARY, ALL};
        enum Print {PRINT,DONT_PRINT};
        assetDbManager(std::string csvfile){
            std::filebuf assetDbcsv;
            if (assetDbcsv.open(csvfile,std::ios::in)){
                std::istream csv_stream(&assetDbcsv);
            while(csv_stream){
                // CSV Parsing.  Could have used external library
                // Assumption is that Python scripts check csv integrity and compatibility for simple parsing by C++
                std::string row;
                std::string IP;
                std::string SKU;
                std::string Longitude;
                std::string Latitude;
                std::string Stationary;
                std::getline(csv_stream,row);
                std::stringstream rowStream(row);
                std::getline(rowStream,IP,',');
                std::getline(rowStream,SKU,',');
                std::getline(rowStream,Longitude,',');
                std::getline(rowStream,Latitude,',');
                std::getline(rowStream,Stationary,',');
                if ( IP.length() > 0){
                AddAsset(IP,SKU,std::stof(Longitude),std::stof(Latitude),std::stoi(Stationary));
            }
        }
            }
            else {
                std::cout << "ERROR: Could not open csv file" << csvfile << std::endl;
                throw -1;
            }
        }
        void AddAsset(std::string IP, std::string SKU, float Longitude, float Latitude, int Stationary ) {
            union PackedCoordinates PackedCoords;
            PackedCoords.coords.Latitude = (signed short) Latitude;
            PackedCoords.coords.Longitude = (signed short) Longitude;
            auto y = [](ASSETDB &assetDb, FASTIP & FastIP, std::string IP, std::string SKU, float Longitude, 
                        float Latitude, int Stationary, union PackedCoordinates PackedCoords ){
                assetDb[SKU][PackedCoords.coords_key][IP].Latitude = Latitude;
                assetDb[SKU][PackedCoords.coords_key][IP].Longitude = Longitude;
                assetDb[SKU][PackedCoords.coords_key][IP].SKU = SKU;
                assetDb[SKU][PackedCoords.coords_key][IP].IP = IP;
                assetDb[SKU][PackedCoords.coords_key][IP].Stationary = Stationary;
                FastIP[IP] = assetDb[SKU][PackedCoords.coords_key][IP];
            };
            y(((Stationary) ? assetDbStationary : assetDbNotStationary),FastIP,IP,SKU,Longitude,Latitude,Stationary,PackedCoords);
        }
        // Returns all assetts with a SKU
        void QueryAssets (std::vector<struct AssetCoordinates> & Assets,std::string SKU, Stationary TransitState, enum Print print){
            auto y = [](ASSETDB &assetDb, std::vector<struct AssetCoordinates> &Assets, std::string SKU,
                        Stationary TransitState, Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    if (assetDb.find(SKU) != 
                        assetDb.end()){
                        for( auto &regions: assetDb[SKU]){
                            for ( auto &ip: assetDb[SKU][regions.first]){
                                Assets.push_back(ip.second);
                            }
                        }
                    }
                }
            };
            y(assetDbNotStationary,Assets,SKU,TransitState,NOT_STATIONARY);
            y(assetDbStationary,Assets,SKU,TransitState,STATIONARY);
            if ( print == PRINT){
                for (auto &asset : Assets){ 
                    std::cout << asset.IP << std::endl;
                }
            }
        }
        // Returns the assett with the IP
        void QueryAssets (std::vector<struct AssetCoordinates> Assets,std::string IP, enum Print print){
            Assets.clear();
            Assets.push_back(FastIP[IP]);
            if ( print == PRINT){
                for(auto &ip: Assets){
                    std::cout << ip.IP << std::endl;
                }
            }
        }
        // Returns the all assets with the SKU in the region of this coordinate
        void QueryAssets (std::vector<struct AssetCoordinates> Assets,std::string SKU, float Latitude, 
                        float Longitude, enum Stationary TransitState, enum Print print){
            union PackedCoordinates Coords;
            Coords.coords.Latitude = (signed short) Latitude;
            Coords.coords.Longitude = (signed short) Longitude;
            auto y = [](ASSETDB & assetDb,std::vector<struct AssetCoordinates> & Assets, union PackedCoordinates Coords, std::string SKU, 
                        enum Stationary TransitState, enum Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    if (assetDb.find(SKU) != 
                        assetDb.end()){
                        if (assetDb[SKU].find(Coords.coords_key) != 
                            assetDb[SKU].end()){
                            for (auto &asset : assetDb[SKU][Coords.coords_key]) { 
                                Assets.push_back(asset.second);
                            }
                        }
                    }
                }
            };
            y(assetDbNotStationary,Assets,Coords,SKU,TransitState,NOT_STATIONARY);
            y(assetDbStationary,Assets,Coords,SKU,TransitState,STATIONARY);
            if (print == PRINT){
                for (auto &asset : Assets){ 
                    std::cout << asset.IP << std::endl;
                }
            }
        }
        // Returns all assets in this region
        void QueryAssets (std::vector<struct AssetCoordinates> & Assets,float Latitude, float Longitude, 
                            enum Stationary TransitState, enum Print print){
            Assets.clear();
            union PackedCoordinates Coords;
            Coords.coords.Latitude = (signed short) Latitude;
            Coords.coords.Longitude = (signed short) Longitude;
            auto y = [](ASSETDB & assetDb,std::vector<struct AssetCoordinates> & Assets, union PackedCoordinates Coords,
                        enum Stationary TransitState, enum Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    for (auto &sku : assetDb) { 
                        if ( Assets.size() > 0){break;}
                        if (assetDb[sku.first].find(Coords.coords_key) != 
                            assetDb[sku.first].end()){
                            for (auto &asset : assetDb[sku.first][Coords.coords_key]) { 
                                Assets.push_back(asset.second);
                            }
                        }
                    }
                }
            };
            y(assetDbStationary,Assets,Coords,TransitState,STATIONARY);
            y(assetDbNotStationary,Assets,Coords,TransitState,NOT_STATIONARY);
            if (print == PRINT){
                for (auto &asset : Assets){ 
                    std::cout << asset.IP << std::endl;
                }
            }
        }
        // Returns all SKUs globally
        void AllSKUs (std::unordered_map<std::string,bool> &unique_SKUs,enum Stationary TransitState, enum Print print){
            auto y = [](ASSETDB & assetDb,std::unordered_map<std::string,bool> & unique_SKUs,
                        enum Stationary TransitState, enum Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    for (auto &sku : assetDb){ unique_SKUs[sku.first] = true; }
                }
            };
            y(assetDbStationary,unique_SKUs,TransitState, STATIONARY);
            y(assetDbNotStationary,unique_SKUs,TransitState, NOT_STATIONARY);
            if ( print == PRINT){
                for (auto &sku : unique_SKUs){ 
                    std::cout << sku.first << std::endl;
                }
            }
        }
        // Returns all existing SKUs in the database for a specific region
        void AllSKUsRegion (std::unordered_map<std::string,bool> & unique_SKUs,float Latitude, float Longitude, 
                            enum Stationary TransitState, enum Print print){
            union PackedCoordinates Coords;
            Coords.coords.Latitude = (signed short) Latitude;
            Coords.coords.Longitude = (signed short) Longitude;
            auto y = [](ASSETDB & assetDb,std::unordered_map<std::string,bool> & unique_SKUs, union PackedCoordinates Coords,
                        enum Stationary TransitState, enum Stationary Transit){
                if ( TransitState == ALL or TransitState == STATIONARY ) {
                    for (auto &sku : assetDb){ 
                        if ((sku.second.find(Coords.coords_key)) != sku.second.end()){
                            unique_SKUs[sku.first] = true; 
                        }
                    }
                }
            };
            y(assetDbStationary,unique_SKUs,Coords,TransitState,STATIONARY);
            y(assetDbNotStationary,unique_SKUs,Coords,TransitState,NOT_STATIONARY);
            if (print == PRINT){
                for (auto &sku : unique_SKUs){ 
                    std::cout << sku.first << std::endl;
                }
            }
        }
        // Returns all existing regions in the database
        void AllRegions (std::unordered_map<unsigned int,bool> & UniqueRegions,enum Stationary TransitState, enum Print print){
            union PackedCoordinates Coords;
            auto y = [](ASSETDB assetDb, std::unordered_map<unsigned int,bool> & UniqueRegions, Stationary TransitState,
                        enum Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    for (auto &sku : assetDb){
                        for (auto &region : sku.second){ UniqueRegions[region.first] = true; }
                    }
                }
            };
            y(assetDbNotStationary,UniqueRegions,TransitState,NOT_STATIONARY);
            y(assetDbStationary,UniqueRegions,TransitState,STATIONARY);
            if ( print == PRINT){
                for (auto &region : UniqueRegions){ 
                    Coords.coords_key = region.first;
                    std::cout << Coords.coords.Latitude <<" " << Coords.coords.Longitude << std::endl;
                }
            }
        }
        ASSET_COORDINATES KeyToCoordinates (unsigned int coords){
            union PackedCoordinates Coords;
            ASSET_COORDINATES ReturnAsset;
            Coords.coords_key = coords;
            ReturnAsset.Latitude = Coords.coords.Latitude;
            ReturnAsset.Longitude = Coords.coords.Longitude;
            return ReturnAsset;
        }
    private:
        typedef std::unordered_map<std::string, 
            std::unordered_map<unsigned int,
                std::unordered_map<std::string,
                        struct AssetCoordinates > > > ASSETDB;
        typedef std::unordered_map<std::string, ASSET_COORDINATES> FASTIP;
        struct ShortCoordinates {
            signed short Longitude;
            signed short Latitude;
        };
        union PackedCoordinates {
            struct ShortCoordinates coords;
            unsigned int coords_key;
        };
        // Hash table for assets that are stationary
        ASSETDB assetDbStationary;
        ASSETDB assetDbNotStationary;
        FASTIP FastIP;
};
int main(){
    std::filebuf assetDbcsv;
    std::unordered_map<unsigned int,bool> Regions;
    std::unordered_map<std::string,bool> UniqueSKUs;
    std::vector<struct assetDbManager::AssetCoordinates> Assets;
    auto start = high_resolution_clock::now();
    assetDbManager assetDb("assetDb.csv");
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << "Database Initialized" << std::endl;
    std::cout << "PERF: Initialization time " << duration.count() << " microseconds" <<std::endl;
    start = high_resolution_clock::now();
    assetDb.AllSKUs(UniqueSKUs,assetDbManager::ALL, assetDbManager::DONT_PRINT);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Unique SKUS: " << UniqueSKUs.size() << std::endl;
    std::cout << "PERF: Time to query unique SKUs " << duration.count() << " microseconds" <<std::endl;
    start = high_resolution_clock::now();
    assetDb.AllRegions(Regions,assetDbManager::ALL,assetDbManager::DONT_PRINT);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Unique Regions: " << Regions.size() << std::endl;
    std::cout << "PERF: Time to query all regions " << duration.count() << " microseconds" <<std::endl;

    /* Profile the time it takes to access the assets of each SKU then compute the average*/
    start = high_resolution_clock::now();
    int total_records = 0;
    for (auto &sku: UniqueSKUs){
        assetDb.QueryAssets(Assets,sku.first,assetDbManager::ALL, assetDbManager::DONT_PRINT);
        total_records += Assets.size();
        Assets.clear();
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "PERF: Average time to query " << UniqueSKUs.size() << " SKUs " 
                << (duration.count() / UniqueSKUs.size()) << " microseconds" <<std::endl;
    std::cout << "   Total records found " << total_records << std::endl;

    /* Profile the time it takes to access assets of each region then compute the average*/
    assetDbManager::ASSET_COORDINATES coords;
    start = high_resolution_clock::now();
    total_records = 0;
    for (auto &region_coords: Regions){
        coords = assetDb.KeyToCoordinates(region_coords.first);
        assetDb.QueryAssets(Assets,coords.Latitude,coords.Longitude,assetDbManager::ALL, assetDbManager::DONT_PRINT);
        total_records += Assets.size();
        Assets.clear();
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "PERF: Average time to query " << Regions.size() << " Regions " 
                << (duration.count() / Regions.size()) << " microseconds" <<std::endl;
    std::cout << "   Total records found " << total_records << std::endl;
    /* Profile the time it takes to access individual IP addresses by fetching them all and computing the average*/
    // First collect all IP addresses from the database
    std::vector<std::string> all_ips;
    for (auto &sku: UniqueSKUs){
        assetDb.QueryAssets(Assets,sku.first,assetDbManager::ALL, assetDbManager::DONT_PRINT);
        for(auto &ips: Assets){
            all_ips.push_back(ips.IP);
        }
        Assets.clear();
    }
    std::cout << "Total assets found " << all_ips.size() << std::endl;
    int MaximumIPsQueried = 100;
    int IPsQueried = 0;
    start = high_resolution_clock::now();
    for(auto &ip: all_ips){
        IPsQueried++;
        assetDb.QueryAssets(Assets,ip,assetDbManager::DONT_PRINT);
        Assets.clear();
        if (IPsQueried > MaximumIPsQueried){ break; }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "PERF: Average time to query " << MaximumIPsQueried << " IP addresses " 
                << ((float)duration.count() / (float)MaximumIPsQueried) << " microseconds" <<std::endl;
    return 0;
}