<?php

/**
 * @file
 * Contains the USPS API Class.
 */

/**
 * The USPS API Class.
 *
 * This class builds and responds to API calls.
 */
class UspsApi {
  // Userid from variable table.
  public $userId;

  // This xml query string.
  public $xmlString;
  public $xmlUrl;

  // Proxy settings.
  protected $proxyEnable;
  protected $proxyServer;
  protected $proxyPort;

  // Debug tools.
  public $dt;

  // Firm name maximum 38 characters.
  public $firmName;
  // Apt or Suite number,
  // maximum 38 characters.
  public $address1;
  // Street address, not PO box or apt #,
  // maximum 38 characters.
  public $address2;
  // City maximum 15 characters.
  public $city;
  // State maximum 2 characters.
  public $state;
  // Zip maximum 5 characters.
  public $zip5;

  // Build response array to return.
  public $r = array();

  /**
   * Constructor.
   *
   * @param array $data
   *    Property values.
   */
  public function __construct($data = array()) {
    // If $data is an array, set the properties.
    if (is_array($data)) {

      // Set the address during instantiation.
      if (count($data) > 0) {
        foreach ($data as $name => $value) {
          $this->$name = $value;
        }
      }

      // Set userid from variable table.
      $this->userId = variable_get('usps_api_id', NULL);
      // If not set, add to error array.
      if ($this->userId == NULL) {
        $this->r['error'] = TRUE;
        $this->r['error_description'] = 'API userId not set in the settings form.';
      }

      // Set proxy settings from variable table.
      $this->proxyEnable = variable_get('curl_proxy_enable', FALSE);
      if ($this->proxyEnable == TRUE) {
        $this->proxyServer = variable_get('curl_proxy_server', NULL);
        $this->proxyPort = variable_get('curl_proxy_port', NULL);
      }

      // Set debug settings.
      $this->dt = variable_get('debug_usps_api', 0);
    }
    // If $data is not an array, set error to true.
    else {
      $this->r['error'] = TRUE;
      $this->r['error_description'] = 'Object not instantiated correctly, parameter must be an array.';
    }
  }

  /**
   * CURL Method.
   */
  protected function curlRequest($url) {
    // Config and start cURL.
    $c = curl_init();
    curl_setopt($c, CURLOPT_URL, $url);
    curl_setopt($c, CURLOPT_HEADER, FALSE);
    curl_setopt($c, CURLOPT_HTTPGET, TRUE);
    curl_setopt($c, CURLOPT_RETURNTRANSFER, 1);

    // Add proxy if configured.
    if ($this->proxyEnable === 'true') {
      curl_setopt($c, CURLOPT_PROXY, $this->proxyServer . ':' . $this->proxyPort);
    }

    // cURL result.
    $result = curl_exec($c);

    // Add debug Xml Response to returned array.
    if (is_string($this->dt['xml_response'])) {
      $this->r['debug_xml_response'] = $result;
    }

    $cr = curl_getinfo($c);

    // Add debug cURL Response to returned array.
    if (is_string($this->dt['curl_response'])) {
      $this->r['debug_curl_response'] = $cr;
    }

    // Check cURL http_code status for error.
    // If error exists, add to error array then return method.
    if ($cr['http_code'] != '200') {
      $this->r['error'] = TRUE;
      $this->r['curl_failed'] = TRUE;
      curl_close($c);
      return $this->r;
    }
    curl_close($c);
    return $result;
  }

  /**
   * Address Standardization Verification API.
   */
  public function validateStandardAddress() {
    // Check if constructor returns errors.
    if (isset($this->r['error']) && $this->r['error'] == TRUE) {
      return $this->r;
    }

    // The xml url.
    $this->xmlUrl = 'http://production.shippingapis.com';
    $this->xmlUrl .= '/ShippingAPI.dll?API=Verify&XML=';

    // Build the api xml queryi string.
    $this->xmlString = '<AddressValidateRequest USERID="' . $this->userId . '">';
    $this->xmlString .= '<Address ID="0">';
    $this->xmlString .= '<FirmName>' . $this->firmName . '</FirmName>';
    $this->xmlString .= '<Address1>' . $this->address1 . '</Address1>';
    $this->xmlString .= '<Address2>' . $this->address2 . '</Address2>';
    $this->xmlString .= '<City>' . $this->city . '</City>';
    $this->xmlString .= '<State>' . $this->state . '</State>';
    $this->xmlString .= '<Zip5>' . $this->zip5 . '</Zip5>';
    $this->xmlString .= '<Zip4></Zip4>';
    $this->xmlString .= '</Address>';
    $this->xmlString .= '</AddressValidateRequest>';

    // Add debug Xml Request to returned array.
    if (is_string($this->dt['xml_request'])) {
      $this->r['debug_xml_request'] = $this->xmlUrl . $this->xmlString;
    }

    // Build xml query url.
    $xu = $this->xmlUrl . rawurlencode($this->xmlString);

    // Make cURL request.
    $result = $this->curlRequest($xu);

    // If curlRequest $result is a valid string.
    if (is_string($result)) {

      // Create an xml object.
      $x = new SimpleXmlElement($result);

      // Check for XML external entity injection.
      foreach ($x->childNodes as $child) {
        if ($child->nodeType === XML_DOCUMENT_TYPE_NODE) {
          $this->r['error'] = TRUE;
          $this->r['error_description'] = 'XML External Entity detected.';
          return $this->r;
        }
      }
    }
    else {
      // Return with cURL error.
      return $this->r;
    }

    // Check if error document was returned.
    if ($x->Address->Error) {
      $this->r['error'] = TRUE;
      $this->r['number'] = (string) $x->Address->Error->Number;
      $this->r['source'] = (string) $x->Address->Error->Source;
      $this->r['description'] = (string) $x->Address->Error->Description;

      // Return with error array.
      return $this->r;
    }

    // Build and return valid address array.
    if ($x->Address->Firmname > 1) {
      $this->r['firmName'] = (string) $x->Address->FirmName;
    }
    if ($x->Address->Address1 > 1) {
      $this->r['address1'] = (string) $x->Address->Address1;
    }
    $this->r['address2'] = (string) $x->Address->Address2;
    $this->r['city'] = (string) $x->Address->City;
    $this->r['state'] = (string) $x->Address->State;
    $this->r['zip5'] = (string) $x->Address->Zip5;
    $this->r['zip4'] = (string) $x->Address->Zip4;

    return $this->r;
  } // validate_standard_address()

}
