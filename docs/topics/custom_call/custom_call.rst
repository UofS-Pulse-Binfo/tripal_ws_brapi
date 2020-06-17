Create Tripal WS BrAPI Custom Call
==================================

Creating a custom call, or other call types, is as easy as setting up parameters
and defining a query or callback function that is responsible for generating
relevant data as the response or result from executing a custom call.

**Class definition**

.. code-block:: php

   class TripalWebServiceBrapiV1Yourcallname extends TripalWebServiceCustomCall {
     // Parameters allowed in this call which can be included in the call as
     // query string, ie: host/brapi/v1/call?parameter 1=value&parameter 2=value...
     public $call_parameter = [
       // Key : Expected value for this key.
       ‘parameter 1’ => ‘data type’,
       ‘parameter 2’ => ‘data type’,
       ‘parameter …’ => ‘data type’,
     ];

     // Keyword used to identify result items.
     // data for most call in BrAPI v1.3 and may vary in newer version.
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


1. Rename the class name using the format defined relating to call class
   filename (titled Yourcallname in the code snippet above).
2. List the parameters that user can apply to request specific items from the
   result. Each parameter can be set to a data type which will ensure that
   only appropriate entries are permitted.

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

3. Define the unit of data and its elements in **$response_field**. Set the
   key to the target BrAPI version number. ie 1.3 or 1.2.
4. Set the **$call_payload_key** to a string value. This variable will render
   as the key in the response. ie data (BrAPI 1.3) and call (BrAPI 2.0) used
   by /calls and /serverinfo calls, repectively.
5. Define the result in the only method of this class getResult().

.. note:: Ensure that the number of items in the data array should match the
   items in the $response_filed. Include a mechanism to handle each parameters
   defined in #2. Parameters requested in the url are available in getResult()
   through the property **$call_asset**

.. code-block:: php

  $this->call_asset[‘parameters’] property and
  $this->call_asset[‘parameters’][‘yourparameter’] to access the value.

6. Save the file.
7. Test your call using host/web-services/brapi/v + version/yourcallname.
