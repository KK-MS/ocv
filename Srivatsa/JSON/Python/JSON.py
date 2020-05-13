import json #importing JSON Module

## This code is to read a string
String = "{\"Shape\": \"Rectangle\",\"Color\": \"Yellow\",\"Text\": \"Kempten 5KM AHEAD\",\"gt\": [{\"gt_lat\": 37.7668,\"gt_long\": -1.223959e+2,\"gt_bearing\":433.656}],\"image\": [{\"img_lat\":425.14253,\"img_long\": 42.1235,\"img_bearing\":475.0102}]}"
String_dict = json.loads(String) # Create a dictionary

print( String_dict) # Print dictionary
# Print specific array
print(String_dict['gt'])
print(String_dict['image'])

## This code is to read a JSON file and print.
## It uses open function to read a file and parsed using json.load which puts the data from the file to a dict.
# ##In this case, the dictionary is data
with open('E:/Kempten/LivingLab/cJSON/anayat_ACC.json')as f:
    data = json.load(f)

    print(data)

## Convert dictionary to JSON.
## Here we will revert the created dictionary to JSON

    revert = json.dumps(data)
    print(revert)

## Writing JSON to a file
## Here we save the data obtained from revert

with open('revert.json','w') as json_file:
    json.dump(revert, json_file)



