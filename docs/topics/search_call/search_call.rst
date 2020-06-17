Create Tripal WS BrAPI Search Call
==================================

Similar to creating a custom call and database call, a search call uses both
**GET** and **POST** request methods to view a query result and request a query,
respectively.

See details below.

**Class definition**

.. code-block:: php

   class TripalWebServiceBrapiV1SearchYourcallname extends TripalWebServiceSearchCall {
     // Parameters allowed in this call which can be included in the call as
     // query string. See example below using CURLOPT_POSTFIELDS option.
     public $call_parameter = [
       // Key : Expected value for this key.
       ‘parameter 1’ => ‘data type’,
       ‘parameter 2’ => ‘data type’,
       ‘parameter …’ => ‘data type’,
     ];

     // Keyword used to identify result items.
     protected $call_payload_key = 'data';

     // Unit of response for this call.
     // With corresponding minor version a response is for.
     // Fields must match item count in the result.

     // See restriction on multiple version in Configuration.
     public $response_field = [
       '1.3' => ['response field 1', 'response field 2', ' response field …'],
       '1.2' => ['...'],
       '2.0' => ['...'],
     ];

     // Chado table, source data.
     public static $chado_table = 'chado.table ie stock'

     // Call parameters as provided in the request url.
     public $call_asset;
     // Class name.
     public $class_name;

     // PREPARE RESULT:
     // Callback to create data.
     public function getResult() {
       // Define values matching the number of elements defined in $response field.
      return [];
     }
   }


   1.	Rename the class name using the format defined relating to call class filename
      (titled Yourcallname in the code snippet above).
   2.	List the parameters that user can apply to request specific items from the
      result. Each parameter can be set to a data type which will ensure that
      only appropriate entries are permitted.

   .. code-block:: php

   .. list-table::
      :widths: 50 50
      :header-rows: 0

      * - **int**
        - (single value) numbers, including 0.
      * - **text**
        - (single value) text, alphanumeric value.
      * - **array-int**
        - (array, multiple values) elements are numbers.
      * - **array-text**
        - (array, multiple values) elements are text value.
      * - **hash-code**
        - (single value) xxxxx-xxxxx-xxxxx-xxxxx-xxxxx alphanumeric format.

   3.	Define the unit of data and its elements in $response_field. Set the key to
      the target BrAPI version number. ie 1.3 or 1.2.
   4.	Set the $call_payload_key to a string value. This variable will render as
      the key in the response. ie data (BrAPI 1.3) and call (BrAPI 2.0) used by
      /calls and /serverinfo calls, repectively.
   5.	Define the result in the only method of this class - getResult().

   .. note:: Ensure that the number of items in the data array should match the
      items in the $response_filed. Include a mechanism to handle each parameters
      defined in #2. Parameters requested in the url are available in getResult()
      through the property **$call_asset**

   .. code-block:: php

      $this->call_asset[‘parameters’] property and
      $this->call_asset[‘parameters’][‘yourparameter’] to access the value.

   6.	Save the file.
   7.	Test your call using host/web-services/brapi/v + version/yourcallname.


  .. note:: This class extends to a different class than the one used in defining
     database calls and custom calls and it is important to specify the source
     table ($chado_table property). Class name now contains a Search keyword as
     described in naming class section. The class this class extends to handles
     both POST (log search request) and GET requests.

  Search call operates differently compared to other calls – custom call and
  database call. Search call needs to POST the call (with parameters) and at this
  stage a hash code is returned. A call can then be requested using the has code
  to view the result or response.

  .. list-table::
     :widths: 50 50
     :header-rows: 1

     * - **POST Search Request**
       - **GET Search Request**
     * - .. code-block:: php

         $ch = curl_init();
         curl_setopt($ch, CURLOPT_URL, "host/tripaltest/web-services/brapi/v1/search/searchcall");
         curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
         curl_setopt($ch, CURLOPT_HEADER, FALSE);
         curl_setopt($ch, CURLOPT_POST, TRUE);

         // Parameter
         curl_setopt($ch, CURLOPT_POSTFIELDS, "{\"parameter\" : [\"value\"]}");

         curl_setopt($ch, CURLOPT_HTTPHEADER, ["Content-Type: application/json"]);
         $response = curl_exec($ch);
         curl_close($ch);
         var_dump($response);   public $call

       - host/tripaltest/web-services/brapi/v1/search/searchcall?searchResultDbId=7FKIa-s29e7-PJJBS-nLL4N-jNoLs
     * - Add parameters in // Parameter line. Parameter in JSON format.
       - Using the hash code returned, get the call response.
     * - RESPONSE: hash code 7FKIa-s29e7-PJJBS-nLL4N-jNoLs
       - Call response JSON.

A copy of the POST request and the hash code can be accessed in the configuration
page. To perform the same search request, use the same hash code to GET request
call to retrieve the same response. This call request and its parameters can be
accessed multiple times so long as the log entry is not deleted.
