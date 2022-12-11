#!/usr/bin/python3
import os

db_query = './assetDb_query.pipe'
db_result = './assetDb_result.pipe'

if not os.path.exists(db_query):
    os.mkfifo(db_query)
if not os.path.exists(db_result):
    os.mkfifo(db_result)
os.system("./assetDb")
