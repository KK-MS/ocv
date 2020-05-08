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

# JSON Fundamentals

To be added

## SRC and LIB Folders

In this folder, there are two folders namely src and lib. The lib folder contains the library needed for JSON Parser (cJSON). 
In src folder, there are two programs namely test.cpp and JSON_parse.cpp. In test.cpp, the program creates a JSON object. The input is text in a structured format. The structure is mentioned in the code and it should be in that format. 
In JSON_parse.cpp, the code takes in a JSON string and converts it to text and prints it on the console window.
