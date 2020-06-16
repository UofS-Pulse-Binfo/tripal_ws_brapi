Create Tripal WS BrAPI Database Call
====================================

Similar to creating a custom call, a database call uses a CHADO table (as source table)
and requires 3 callback function or method to handle querying the source table.
Two of the methods represent query type that matches the configuration settings
applied to calls – by type_id or by value of prop table.

**Class definition**

.. code-block:: php

   class TripalWebServiceBrapiV1Yourcallname extends TripalWebServiceDatabaseCall {
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

     // Chado table, source data.
     public static $chado_table = 'chado.table ie stock'

     // Call parameters as provided in the request url.
     public $call_asset;
     // Class name.
     public $class_name;

     // PREPARE RESULT:
     // Callback to create data.

     // Unfiltered result.
     public function getResult() {
       // Define values matching the number of elements defined in $response field.
       return [];
     }

     // Result matching type_id
     public function getResultByTypeid() {
       $result = '';
       return $result;
     }

     // Result matching value in property table.
     public function getResultByPropertyTable() {
       $result = '';
       return $result;
     }
   }

   .. note:: NOTE: This class extends to a different class than the one used in
      defining custom calls and it is important to specify the source table
      ($chado_table property).

   To apply parameters (defined in the class) to a call, include each item with
   the request url.

   For example:
   **Host/web-services/brapi/v1/germplasm?commonCropName=Lentil**

   1.	Rename the class name using the format defined relating to call class
      filename (titled Yourcallname in the code snippet above).
   2.	List the parameters that user can apply to request specific items from the
      result. Each parameter can be set to a data type which will ensure that only appropriate entries are permitted.

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

   3.	Define the unit of data and its elements in $response_field. Set the key
      to the target BrAPI version number. ie 1.3 or 1.2.
   4.	Set the **$call_payload_key** to a string value. This variable will render
      as the key in the response. ie data (BrAPI 1.3) and call (BrAPI 2.0) used
      by /calls and /serverinfo calls, repectively.
   5.	Construct query that will correspond to each case - **Unfiltered, By type_id
      and By value in property table**. The last two options only apply when desired.

   .. note:: Ensure that the number of items in the data array should match the
      items in the $response_filed.

      Include a mechanism to handle each parameters defined in #2. Parameters
      requested in the url are available in each methods through the property
      **$call_asset**

   .. code-block:: php

      $this->call_asset[‘parameters’] property and
      $this->call_asset[‘parameters’][‘yourparameter’] to access the value.

   6.	Save the file.
   7.	Test your call using host/web-services/brapi/v + version/yourcallname.
