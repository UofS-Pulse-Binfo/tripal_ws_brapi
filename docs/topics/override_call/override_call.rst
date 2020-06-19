Create Tripal WS BrAPI Override Call or Call Alias
==================================================

In some cases, calls from different versions implement the same process carried
out by an existing call. To eliminate the need to copy and paste codes,
call can point to an existing call and function identical to the source call.

**Class definition**

.. code-block:: php

   class TripalWebServiceBrapiV2Serverinfo extends TripalWebServiceCustomCall {
     // Call parameters as provided in the request url.
     public $call_asset;

     // Override the base class payload key.

     // Keyword used to identify result items.
     // data for most call in BrAPI v1.3 and may vary in newer version.
     protected $call_payload_key = 'call';

     // Call existing and identical call already setup.
     // Declare existing call.
     //
     // module name / char v + major version number / call name.
     //
     // See table below for the actual alias of existing calls in this module.
     public static $is_alias_of = 'tripal_ws_brapi/v1/calls';
   }


1. To override the $call_payload_key of the source call, set the value of the
   property **$call_payload_key**.
2. Extend the class identical to the type of call of the source call.
3. Finally, set the source call by specifying the module it is hosted followed
   by the version number then the call title. For example, the call – mygermplasm
   that mimics germplasm call, located in tripal_ws_brapi module:

**tripal_ws_brapi/v1/germplasm**

In cases when call wants to point to a search call, add search/ level between
version number and the call title.

**tripal_ws_brapi/v1/search/germplasm**

.. note:: Ensure correct override configuration settings when implementing an
   override, by using the exact same call title but is hosted by module external
   to tripal_ws_module.

.. note:: When implementing from an external module, prefix the class and class
   filename with **External** keyword (see naming class section).

For example:

.. code-block:: php

  class ExternalTripalWebServiceBrapiV2Serverinfo extends TripalWebServiceCustomCall

Summary table of existing Tripal WS BrAPI Calls when implementing a call Alias and/or Override:

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - **tripal_ws_brapi Calls**
     - **Call alias property - $is_alias_of:**
   * - **v1/calls**
     - tripal_ws_brapi/v1/calls
   * - **v1/commoncropnames**
     - tripal_ws_brapi/v1/commoncropnames
   * - **v1/crops**
     - tripal_ws_brapi/v1/crops
   * - **v1/germplasm**
     - tripal_ws_brapi/v1/germplasm
   * - **v1/search/germplasm**
     - tripal_ws_brapi/v1/search/germplasm
   * - **Other calls**
     - TO DO.
