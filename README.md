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
     
     
     
       
       
    
    
  
  
      
