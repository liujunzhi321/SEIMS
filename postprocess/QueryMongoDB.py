from datetime import datetime
from pymongo import Connection

# i have updated and included the complete code 
client = Connection('localhost', 27017)
db = client['database'] # your database name
inoshare = db['inoshare']


# convert your date string to datetime object
start = datetime(2014, 9, 24, 7, 51, 04)
end = datetime(2014, 9, 24, 7, 52, 04)

inoshare.find( {'id_no': 1, 'datahora': {'$lt': end, '$gte': start}, 'porta': 'A0'})