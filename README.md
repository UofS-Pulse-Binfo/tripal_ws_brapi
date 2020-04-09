**3rd Commit - Search Calls**

This commit enables this module to perform search request using POST and GET methods. A POST requests along with parameters, are cached to allow repeated search request to be carried out without having to re-enter request and its relevant parameters. This commit includes a search functionality setup to germplasm call.

**Search Request/Call Method**

1. When a call supports search, it can be accessed by adding /search level.
For example: germplasm call becomes germplasm/search

2. Create search request using POST method.

```
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "host//web-services/brapi/v1/germplasm/search"); // link to the call that supports search.
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_HEADER, FALSE);
curl_setopt($ch, CURLOPT_POST, TRUE);

// Define parameters to the search request.
// Fill in array values to filter your search with.
curl_setopt($ch, CURLOPT_POSTFIELDS, "{
  "accessionNumbers": [],
  "commonCropNames": [],
  "germplasmDbIds": [],
  "germplasmGenus": [],
  "germplasmNames": [],
  "germplasmPUIs": [],
  "germplasmSpecies": [],
  "page": 0,
  "pageSize": 0
}");

curl_setopt($ch, CURLOPT_HTTPHEADER, ["Content-Type: application/json"]);
$response = curl_exec($ch);
curl_close($ch);
var_dump($response);
```
3. A hash code titled searchRequestDbId will be returned as the response to the the POST request.

4. Access the link host//web-services/brapi/v1/germplasm/search?searchRequestDbId=HASH CODE in #3
to view the result. The hash code is cached in the system to ensure quick access of the search result without
retyping the POST request.

NOTE: The post can be typed in a .php file and be executed in your php-enabled site, alternatively, using your
Drupal devel/php, paste your code and click execute to achieve the same result.


**Test germplasm search.**

1. Using the dataset from Commit #2. Search stock table for stocks with name 'olivia' and 'emma'.

2. Create POST request
```
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "host//web-services/brapi/v1/germplasm/search"); 
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_HEADER, FALSE);
curl_setopt($ch, CURLOPT_POST, TRUE);
curl_setopt($ch, CURLOPT_POSTFIELDS, "{"germplasmNames": ['olivia', 'emma']}");
curl_setopt($ch, CURLOPT_HTTPHEADER, ["Content-Type: application/json"]);
$response = curl_exec($ch);
curl_close($ch);
var_dump($response);
```
3. Copy and paste to devel/php or create a .php file.

4. Locate in response and copy searchResultDbId code/id returned in #2

5. replace or paste searchResultDbId to id in #4 in this link host//web-services/brapi/v1/germplasm/search?searchRequestDbId=HASH CODE/ID to view result.


**Create a search call**

1. Using the sample call setup in Commit #2.

2. Copy class file of My stocks into calls/v1/search and rename the class it extends to TripalWbeServiceSearchCall.

3. Define parameters. For this example let's allow search to filter by stock names. in $call_parameter we add

```'stockNames' => 'array-text'```

array-text tells the module that this parameter expects an array of text values.

4. Define response field used when showing result of the search (GET).
For this example we want to show the stock id, name and uniquename thus we add to 
```
$response_field (assuming call is v1.3)
'1.3' => ['stockId', 'name', 'uniqueName']
```
5. Save work.

6. Calls share query function between DB calls and Search call to maximize code reusability, therefore it need to know that
 call expects parameters defined in the search class that will become part of the where clause of the query.

  in includes/db.inc, update mystocks query function.

6. Follow Test above but use /mystock/search as the search link of the request.




**2nd Commit - Database Calls.**

This commit adds functionality that enables calls to query and return data stored in a database table. Inlcuded in this commit are calls crops (BrAPI 1.2), commoncropnames (BrAPI 1.3) and germplasm call (BrAPI 1.3). 

**Configuration:**

Important module settings such as supported version and call-specific configuration can be access using the configuration form through the link: host/admin/tripal/extension/tripal_ws_brapi/configure.

Before any calls mentioned above can be accesses, it is important to configure the module.

Configuration options.

**A. General Module Configuration** - described in the first commit.

**B. Versions** - create BrAPI version number that your site will support.

**C. Call** - configure how a specific call performs database query.


In version tab, create a version 1.3 and click add version to create BrAPI v1.3 configuration and sample calls mentioned above can be subsequently accessed through: host/web-services/brapi/v1/call - commoncropnames or germplasm. If v1.2 is
available and is set to default BrAPI v1.2 crops can be accessed as well.

In the call tab, values are based on the values (typeid or prop table) of the source table and administrator can select
appropriate value to serve as additional filter to the query. This eliminates the need for enforcing a term or hard-written
configuration.

**Create a DB call**

Note: a completed call is available in v1/mystock directory of this commit.

Our call profile.

Call name: mystocks - host/web-services/brapi/v1/mystocks

Source table: chado.stocks

Typeid: my stocks in NULL cv (cv_id = 1)

Prop: cool stocks in NULL cv (cv_id = 1)


Step 1: Create required terms.
  ``` 
  tripal_insert_cvterm(
    array('id' => 'test:' . 'my stocks', 'name' => 'my stocks', 'cv_name' =>'null'),
  );
  
  tripal_insert_cvterm(
    array('id' => 'test:' . 'cool stocks', 'name' => 'cool stocks', 'cv_name' =>'null')
  ); 
  ```
  Later, we add rows to stockprop and set the type id to cool stocks and value to awesome or not awesome.

Step 2: Register call in the config file under map array.

Step 3: Insert sample stock rows and tag using terms created above.
```
// Inserts 25 sample stocks.
$mystocks= array('olivia', 'emma', 'aria', 'ava', 'mia', 'mila', 'chloe', 'zoe', 'abigail', 'ariana', 'elena', 'eva', 'sarah', 'naomi', 'sadie', 'liam', 'noah', 'lucas', 'leo', 'max', 'dylan', 'landon', 'marcus', 'david', 'rowan');

$organism = ; // Change to organism # in your database.
$type_id  =  ; // Change to type id created

$propvaltype = ; // Change to prop value created above.
$awesome = array(1 => 'awesome', 2 => 'not awesome');

foreach($mystocks as $i => $s) {
  $arr_stock = array(
    'organism_id' => $organism,
    'name' => $s,
    'uniquename' => 'uname-' . $s,
    'type_id' => $type_id,
    'dbxref_id' => null,
    'description' => 'description ' . $s,
  );
  $stock = chado_insert_record('stock', $arr_stock);
  
  $v = ($i < 15) ? 1 : 2;
  $arr_stockprop = array(
    'stock_id' => $stock['stock_id'],
    'type_id' => $propvaltype,
    'value' => $awesome[$v],
  );
  chado_insert_record('stockprop', $arr_stockprop);
}
```  
Code will insert 25 stocks which can be configured to show all stocks where the type_id is my stocks or through the property
table value field to be either awesome or not awesome.

Step 4: Create your query in db.inc file to match the call name by adding 'tripal_ws_db_' prefix, in our call the final
function name for the query is tripal_ws_db_mystocks(). Configuration values can be accessed through the only parameter
to the function and query can be customized accordingly for each configuration. See mystocks db query function in db.inc.

Step 5: Access call through host/web-services/brapi/v1/mystocks.


First Commit - Custom Calls

This is the first commit of Tripal WS BrApi module. At the moment, this only supports calls that do not involve any data stored
in a database table, such as BrAPI 1.3 calls or BrAPI v2 serverinfo.

To test:
  1. Copy tripal_ws_brapi module into /modules directory and enable it in your Drupal modules list.
     Upon enabling, a number of variables are being set namely:
     
     A. name of vocabulary, brapi in this instance.
     
     B. the name of the module
     
     C. result set limit, number of items per page shown to the user in the response.
     
     D. supported version number, 1.3 in this instance
     
     E. supported HTTP request method, since custom calls are limitted only to showing data, it set to GET method by default.
     
     F. menu level, to set an arbitrary number of levels between the host and BrAPI level. For instance
     host/web-services/lentil/brapi... or host/brapi...
     
     In this first commit, changing any settings will involve calling drupal function variable_set(setting variable), but will
     eventually be moved to a configuration page once completed. Please see .install file for complete variable names.
  
  
  About the Call: BrAPI 1.3 calls or BrAPI 2 serverinfo:
  
    1. This call will return all available calls in the server.
    
    2. Call has predefined parameter 'page' that can be used to request specific page number.
      ie. host/web-services/brapi/v1/calls?page=1 or host/web-services/brapi/v2/serverinfo?page=1
      
      
  Creating a Custom Call:
  
  A sample custom call - 'about' is available as reference under calls/v1/about, which lists a group of person.
  
  1. Depending on which version your host will support, create a direcory either in V1 or V2/module directory/ and name
  it to the desired call name. ie- developers, plants etc (in lowercased).
  
  2. Duplicate and move or Copy and paste class file in 'about' call example and rename the VX and Call name part to the version
  number your host is supporting and the call name to the desired name, respectively.
  
  For example: supported version is 1.3 and call name is Scientists.
  The file should be TripalWebServiceBrapiV1Scientist.inc (NOTE it only uses the major version)
  
  3. Edit the file and rename the class name to match the filename excluding the .inc part.
  
  4. Set parameters
  
    $call_parameter:
    
    If you want the result paged, add the key 'page' and set the expected value to 'int'.
    
    $call_payload_key:
    
    Tag payload with a custom key by setting this variable. This can be used to label result set to data (BrAPI 1.3) or calls (Brapi 2) 
    or simply people as in our Scientist example.
    
    $response_field:
    
    Create an array definition that will form as the basic unit of data in the response.
    
  5. Prepare payload.
  
    1. $metadata variable - set this to empty array if metadata information is not required.
       To implement metadata information, create an array with keys totalCount, pageSize, totalPages and currentPage, the
       system will automate these keys with relevant values.
       
     2. $payload variable - construct data item using the response field variable as the pattern or format.
     
     3. When both properties are set, call buildResponse() method with this inforation as the parameter.
     
   6. Load your custom call - host/web-services/brapi/v Version Number/Call Name (scientist)  
     
     
     
       
       
    
    
  
  
      
