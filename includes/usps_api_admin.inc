<?php

/**
 * @file
 * Admin settings form for USPS API.
 */

/**
 * Builds the admin form for configuring USPS API.
 *
 * @return array
 *   Drupal form for USPS API settings.
 */
function usps_api_settings_form() {
  $form = array();

  // API fieldset.
  $form['api'] = array(
    '#title' => t('USPS API Authentication'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );
  $form['api']['usps_api_id'] = array(
    '#type' => 'textfield',
    '#title' => t('API User ID'),
    '#description' => t('Enter your API User ID.'),
    '#required' => TRUE,
    '#default_value' => variable_get('usps_api_id'),
  );

  // Proxy fieldset.
  $form['advance'] = array(
    '#title' => t('Proxy Server Settings'),
    '#type' => 'fieldset',
    '#description' => t('Enable the proxy to allow cURL to make external API calls.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advance']['curl_proxy_enable'] = array(
    '#type' => 'radios',
    '#title' => t('Enable Proxy Server'),
    '#options' => array(
      'true' => t('On'),
      'false' => t('Off'),
    ),
    '#default_value' => variable_get('curl_proxy_enable', 'false'),
  );
  $form['advance']['curl_proxy_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Proxy Server'),
    '#description' => t('Enter your proxy server address.'),
    '#default_value' => variable_get('curl_proxy_server'),
  );
  $form['advance']['curl_proxy_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Proxy Port'),
    '#description' => t('Enter your proxy port.'),
    '#default_value' => variable_get('curl_proxy_port'),
    '#size' => 20,
  );

  // Develeper debug fieldset.
  $form['dev'] = array(
    '#title' => t('Developer Debug Tools'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Debug checkboxes defaults.
  $debug_defaults = variable_get('debug_usps_api', 0);
  $form['dev']['debug_usps_api'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable debug variables in returned array.'),
    '#options' => array(
      'xml_request' => t('Xml Request'),
      'xml_response' => t('Xml Response'),
      'curl_response' => t('cURL Response'),
    ),
    '#default_value' => $debug_defaults,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  // Set form validation.
  $form['#validate'][] = 'usps_api_settings_form_validate';
  return $form;
}

/**
 * Settings form submit handler.
 */
function usps_api_settings_form_submit($form, $form_state) {
  // Define fields and form_state values.
  $v = $form_state['values'];
  $fields = array(
    'usps_api_id',
    'curl_proxy_enable',
    'curl_proxy_server',
    'curl_proxy_port',
    'debug_usps_api',
  );
  // Loop through fields to save or delete.
  foreach ($fields as $f) {
    if (!empty($v[$f])) {
      variable_set($f, $v[$f]);
    }
    else {
      // If empty, delete value not to save a null value.
      variable_del($f, $v[$f]);
    }
  }
  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Form validation.
 */
function usps_api_settings_form_validate($form, &$form_state) {
  $v = $form_state['values'];

  // Proxy form must be completed if enabled.
  if ($v['curl_proxy_enable'] === 'true') {

    // Proxy server address.
    if ($v['curl_proxy_server'] == NULL) {
      form_set_error('curl_proxy_server', t('Please enter a Proxy Server address.'));
    }
    // Proxy port.
    if ($v['curl_proxy_port'] == NULL) {
      form_set_error('curl_proxy_port', t('Please enter a Proxy Port.'));
    }
  }
}
