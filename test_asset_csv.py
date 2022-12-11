#!/usr/bin/python3
import random
import string
import csv
asset_file = 'assetDb.csv'
asset_db = {}
# Test asset csv file and populate database
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
exit(0)
# print the list of IPs in each SKU
for SKU in asset_db:
    print("SKU: "+SKU)
    for IP in asset_db[SKU]:
        print("   "+IP+" : ",asset_db[SKU][IP])