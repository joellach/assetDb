#!/usr/bin/python3
import random
import string
import csv
import argparse
import os


parser = argparse.ArgumentParser(description='assetDb query profile test tool')
parser.add_argument('-assets',type=int,help="Total number of assets to generate")
parser.add_argument('-skus',type=int,help="Total number of SKUs")
parser.add_argument('-stationary',type=int,help="0-100, percentage of stationary assets")
parser.add_argument('-regions',type=int,help="Total number of regions to generate")
args = parser.parse_args()

if args.assets:
    trackers = args.assets
else:
    trackers = 100
if args.skus:
    SKUs = args.skus
else:
    SKUs = 4
if args.regions:
    regions = args.regions
else:
    regions = 10
if args.stationary:
    if( (args.stationary > 100) or (args.stationary < 0)):
        print("ERROR: specify the stationary assetss as an integer between 0-100")
        exit(2)
    percentage_IPs_stationary = args.stationary
else:
    percentage_IPs_stationary = 90
region_list = []
SKU_list = []
SKU_size = 12
IP_size = 32
IPs = {}
print("Creating an assetDb.csv file with",trackers,"assets, ",SKUs," SKUs, ",regions," regions")
print("   ",percentage_IPs_stationary,'% of assets will be stationary')
#Create random SKUs to choose from
for SKU in range(SKUs):
    SKU_list.append(''.join(random.choices(string.ascii_uppercase + string.digits, k=SKU_size)))
for region in range(regions):
    latitude = str(random.uniform(-180,180))
    longitude = str(random.uniform(-90,90))
    region_list.append(latitude+","+longitude)
#Create CSV file with random SKUs/IPs/coordinates/movement information
print("Creating assetDb.csv file")
with open("assetDb.csv", 'w') as f:
    for t in range(trackers):
        latitude = str(random.uniform(-180,180))
        longitude = str(random.uniform(-90,90))
        SKU = SKU_list[random.randint(0,(SKUs-1))]
        region = region_list[random.randint(0,(regions -1))]
        stationary = "0"
        if (random.randint(0,100) < percentage_IPs_stationary ):
            stationary = "1"
        while (1):
            IP = ''.join(random.choices(string.digits, k=IP_size))
            # Make sure IP address is unique
            if not IP in IPs:
                break
        f.write(IP+","+SKU+","+region+","+stationary+ "\n")
asset_file = 'assetDb.csv'
asset_db = {}
# Test asset csv file and make sure there are no serious issues
print("Testing CSV file for integrity")
with open(asset_file, newline='') as asset_csv_file:
    assets = csv.reader(asset_csv_file)
    for asset in assets:
        if not asset[1] in asset_db:
            asset_db[asset[1]] = {}
        if asset[0] in asset_db[asset[1]]:
            print("ERROR: duplicate IP address", asset[0])
            exit(1)
        else:
            asset_db[asset[1]][asset[0]] = asset[2:]
print("Testing Database performance")
os.system("./assetDb")
exit(0)
