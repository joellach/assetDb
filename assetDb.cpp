#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>
class assetDbManager {
    public:
        struct AssetCoordinates {
            float Longitude;
            float Latitude;
            int Stationary;
            std::string SKU;
            std::string IP;
        };
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
            auto y = [](ASSETDB &assetDb, std::string IP, std::string SKU, float Longitude, 
                        float Latitude, int Stationary, union PackedCoordinates PackedCoords ){
                assetDb[SKU][PackedCoords.coords_key][IP].Latitude = Latitude;
                assetDb[SKU][PackedCoords.coords_key][IP].Longitude = Longitude;
                assetDb[SKU][PackedCoords.coords_key][IP].SKU = SKU;
                assetDb[SKU][PackedCoords.coords_key][IP].IP = IP;
                assetDb[SKU][PackedCoords.coords_key][IP].Stationary = Stationary;
            };
            y(((Stationary) ? assetDbStationary : assetDbNotStationary),IP,SKU,Longitude,Latitude,Stationary,PackedCoords);
        }
        // Returns all assetts with a SKU
        void QueryAssets (std::vector<struct AssetCoordinates> Assets,std::string SKU, Stationary TransitState, enum Print print){
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
            auto y = [] (ASSETDB &assetDb, std::vector<struct AssetCoordinates> &Assets, std::string IP){
                for (auto &sku: assetDb){
                    for( auto &regions: assetDb[sku.first]){
                        if (assetDb[sku.first][regions.first].find(IP) != 
                            assetDb[sku.first][regions.first].end()){
                            Assets.push_back(assetDb[sku.first][regions.first][IP]);
                        }
                    }
                }
            };
            y(assetDbStationary, Assets,IP);
            y(assetDbNotStationary, Assets,IP);
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
            union PackedCoordinates Coords;
            Coords.coords.Latitude = (signed short) Latitude;
            Coords.coords.Longitude = (signed short) Longitude;
            auto y = [](ASSETDB & assetDb,std::vector<struct AssetCoordinates> & Assets, union PackedCoordinates Coords,
                        enum Stationary TransitState, enum Stationary Transit){
                if ( TransitState == ALL or TransitState == Transit ) {
                    for (auto &sku : assetDb) { 
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
    private:
        typedef std::unordered_map<std::string, 
            std::unordered_map<unsigned int,
                std::unordered_map<std::string,
                        struct AssetCoordinates > > > ASSETDB;
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
};
int main(){
    std::filebuf assetDbcsv;
    std::unordered_map<unsigned int,bool> UniqueRegions;
    std::unordered_map<std::string,bool> unique_SKUs;
    std::vector<struct assetDbManager::AssetCoordinates> Assets;
    assetDbManager assetDb("assetDb.csv");
    std::cout << "Database Initialized" << std::endl;
    //assetDb.AllSKUs(unique_SKUs,assetDbManager::ALL, assetDbManager::PRINT);
    //assetDb.AllRegions(UniqueRegions,assetDbManager::ALL,assetDbManager::PRINT);
    //assetDb.AllSKUsRegion(unique_SKUs,22,108,assetDbManager::ALL, assetDbManager::PRINT);
    //assetDb.QueryAssets(Assets,22.0,108.0,assetDbManager::ALL, assetDbManager::PRINT);
    //assetDb.QueryAssets(Assets,"VA9TDG7ES9H7",22,108,assetDbManager::ALL, assetDbManager::PRINT);
    assetDb.QueryAssets(Assets,"VA9TDG7ES9H7",assetDbManager::ALL, assetDbManager::PRINT);
    //assetDb.QueryAssets(Assets,"35378190207497538290640158540254",assetDbManager::PRINT);
    //assetDb.QueryAssets(Assets,"35378190207497538290640158540254",assetDbManager::PRINT);
    return 0;
}