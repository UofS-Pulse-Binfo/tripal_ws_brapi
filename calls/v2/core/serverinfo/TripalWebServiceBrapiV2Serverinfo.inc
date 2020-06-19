<?php
/**
 * Call: Calls, class definition.
 * The '/calls' call is used to find the available BrAPI calls on a particular server.
 */
 class TripalWebServiceBrapiV2Serverinfo extends TripalWebServiceCustomCall {
   // Call parameters as provided in the request url.
   public $call_asset;
   
   // Override the base class payload key.
   protected $call_payload_key = 'call';

   // Call existing and identical call already setup.
   // Declare existing call.
   //
   // module name / char v + major version number / call name.
   public static $is_alias_of = 'tripal_ws_brapi/v1/calls';
 }
