#!/usr/bin/python3
import random
import string

trackers = 100 
SKUs = 4
regions = 20
region_list = []
SKU_list = []
SKU_size = 12
IP_size = 32
IPs = {}
percentage_IPs_stationary = 90
#Create random SKUs to choose from
for SKU in range(SKUs):
    SKU_list.append(''.join(random.choices(string.ascii_uppercase + string.digits, k=SKU_size)))
for region in range(regions):
    latitude = str(random.uniform(-180,180))
    longitude = str(random.uniform(-90,90))
    region_list.append(latitude+","+longitude)
#Create CSV file with random SKUs/IPs/coordinates/movement information
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