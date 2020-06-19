<?php
/**
 * Call: Calls, class definition.
 * List the common crop names for the crops available in a database server.
 */
class TripalWebServiceBrapiV2Commoncropnames extends TripalWebServiceDatabaseCall {
  // Call parameters as provided in the request url.
  public $call_asset;
  
  // Call existing and identical call already setup.
  // Declare existing call.
  //
  // module name / char v + major version number / call name.
  public static $is_alias_of = 'tripal_ws_brapi/v1/commoncropnames';
}
