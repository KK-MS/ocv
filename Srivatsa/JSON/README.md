# JSON

JSON stands for JavaScript Object Notation. It was created by Doughlas Crockford.

![JSON_logo](https://user-images.githubusercontent.com/56631153/80986215-8f2eba00-8e30-11ea-9568-75f689261fff.png) Logo of JSON

## What is JSON? 

JSON (JavaScript Object Notation) is a syntax for storing and exchanging data. It is a text, written in JavaScript Object Notation. It is lightweight. JSON basically converts all the objects used in JavaScript (or any language) into text.

* As per Wikipedia, JSON is an open standard file format and data interchange format, that uses human-readable text to store and transmit dataobjects consisting of attribute-value pairs and array data types.

* It is a very common data type used in many applications such as Microsoft Azure (where they use JSON defined templates in order to deploy the application in the cloud).
It is also being considered as a replacemnt for XML (Extensible Mark-up Language) in AJAX (Asynchronous Javascript and XML) Systems.

* JSON is a language-independent data format. It was derived from JavaScript, but many programming languages include code to generate and parse JSON format data. JSON Filenames use the extension - **"file.json"**

## Why JSON? 

JSON is 

* Less Verbose :- JSON has a more compact style than XML, and is often more readable.
The light weight approach of JSON can make significant improvements.

* Fast :- The XML software parsing process can take a ling time. JSON uses less data overall, thus reducing the cost and in turn increases the parsing speed.

* Readable :- The JSON structure is straightforward and readable, thus having easier time mapping to domain objects.

* Structured Data :- JSON uses Map data structure rather than XML's tree based structure. In some situation, key/value pairs limit the functionality, but we end up having a predictable and easy to understand data model.

# JSON vs XML (Comparision)

XML stands for Extensible MarkUp Language. Both the formats are used for the same purposes.

| JSON | XML |
|------|------|
|Fetch a JSON String | Fetch an XML Document|
|JSON.Parse the JSON String | Use the XML DOM to loop through the document|
|          | Extract values and store in variable|

JSON is far better as XML is much more difficult to parse. 
In case of JSON, it is parsed into a ready-to-use JavaScript object making it easier and simple.

For Example:- In case of AJAX applicatiosn, JSON is comparitively faster compared to XML. 
The process of JSON and XML are shown in the table.

# Syntax Rules in JSON

JSON syntax is a subscript of JavaScript syntax. Thus it is very similar to JS.

* Defining an object in JSON

In JSON, the keys are also written in double quotation marks

Example:- {"Name":"XX"}

Here in the above example, the key is **"name"** and value is **"XX"**

Some rules to be considered while defining an object is as follows

* Data is in name/value pairs

* Data is separated by comma.

For example:- {"Name":"XX", "Age":"1000"}

* Curly braces hold objects {}

* Square brackets holds array []

## FOLDERS

In JSON folder, there are 3 folders which are as follows

#### src

This folder contains all the source code needed for JSON parsing. There are 3 files 

* [test.cpp](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/src/test.cpp):- In this file, it showcases about the creation of JSON object.

* [JSON_Parse.cpp](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/src/JSON_Parse.cpp):- In this file, the input is a JSON string and outputs the text from the string. It is done by parsing JSON in C

* [JSON_parse_file_read.cpp](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/src/JSON_parse_file_read.cpp):- In this file, the input is given as a JSON file and outputs the text from the file. It is done by parsing JSON in C.

#### Lib

This folder contains the libraries needed for parsing JSON in C/C++. There are two files

* [cJSON.h](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/lib/cJSON.h):- This file must be included in the code. This is important as this helps in JSON parsing. 

* [cJSON.c](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/lib/cJSON.c):- In order to increase the functionalities, this file must be attached in your project source and this file needs to be built along with the source code. This file contains most of the functions and thus improves the functionalities.

#### JSON files

The two JSON files used are as follows. these files are used to run the [JSON_parse_file_read.cpp](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/src/JSON_parse_file_read.cpp) file. 

* [anayat_ACC.json](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/JSON_Files/anayat_ACC.json)

* [anayat_LKAS.json](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/JSON_Files/anayat_LKAS.json)

## C++

Although cJSON can be used for c and c++, it is not possible to run the cJSON library on c++ compiler. Thus it must be taken care that the library is initially run on c compiler and then link to the c++ code. 

Another way is to use nlohmann library for c++ 

[nlohmann/json](https://github.com/nlohmann/json) : The functionalities of this library is fairly similar to cJSON, further details on functions can be obtained by clicking on the link.

## Python

In python, the json library is imported by using the following command.

"import json"

In Python folder, there are two files 

* [JSON.py](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/Python/JSON.py)

This python file contains basic demo on how to use JSON library. It involves reading a hard-coded string, reading a file and also convert the text to JSON string and write a file

* [revert.json](https://github.com/KK-MS/ocv/blob/ZIM-221-JSON/Srivatsa/JSON/Python/revert.json)

This file is obtained when python code was run to write the text to JSON string format


## References 

* [JSON basics](https://www.youtube.com/watch?v=uw_rP5bV9r0) 

* [cJSON Library](https://github.com/DaveGamble/cJSON) for additional references

