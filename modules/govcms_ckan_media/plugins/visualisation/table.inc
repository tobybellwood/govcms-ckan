<?php
/**
 * @file
 * Basic table visualisation (default).
 */

$plugin = array(
  'title' => t('Table visualisation'),
  'ckan_filters' => array(
    'search' => NULL,
    'filters' => NULL,
    'column_overrides' => array(),
  ),
  'settings' => array(
    'keys' => array(),
    'labels' => NULL,
    'split' => NULL,
    'ckan_filters' => array(
      'search' => NULL,
      'filters' => NULL,
    ),
    'column_overrides' => array(),
    'merge_settings' => array(
      'merge_keys' => NULL,
    ),
  ),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_media_table_view($file, $display, $config) {
  $element = array();
  $chart_class = 'ckan-table';
  if (isset($file->resource_id)) {
    $ckan_search = govcms_ckan_get_config_value($config, 'ckan_filters/search');
    $ckan_filters = govcms_ckan_get_config_value($config, 'ckan_filters/filters');
    $response = govcms_ckan_client_request_records($file->resource_id, $ckan_search, $ckan_filters);
  }
  elseif (!empty($file->data)) {
    $response = new stdClass();
    $response->data = $file->data;
    $response->valid = TRUE;
  }

  // If failure, provide error message.
  if ($response->valid !== FALSE) {

    // Setup our configuration.
    $keys = array_filter($config['keys']);
    $column_overrides = govcms_ckan_display_parse_column_overrides(govcms_ckan_get_config_value($config, 'column_overrides'));

    // Attributes for the table.
    $attributes = array(
      'class' => array($chart_class),
    );

    // Parse the data.
    $parser = govcms_ckan_dataset_parser($response->data);
    $parser
      ->setKeys($keys)
      ->setLabelKey($config['labels'])
      ->setGroupKey($config['split'])
      ->setMergeKeys($config['merge_settings']['merge_keys'])
      ->setHeaderSource('keys')
      ->setTableAttributes($attributes);

    if ($column_overrides) {
      $parser->setColumnAttributes($column_overrides);
    }

    // Return the table in a wrapper.
    $element = array(
      '#theme' => 'ckan_display_table_wrapper',
      '#tables' => $parser->parse(),
      '#show_titles' => !empty($config['split']),
    );

    // Add JS to the table.
    govcms_ckan_media_expose_table_selector('.' . $chart_class);
  }

  return $element;
}

/**
 * Configure form callback.
 */
function govcms_ckan_media_table_configure($plugin, $form, $form_state, $config) {
  // Get default key elements.
  $config_form = govcms_ckan_media_visualisation_default_key_config($form, $form_state, $config, 'all');

  // Allow for an empty selection on label key.
  $config_form['labels']['#empty_option'] = t('None');

  // Return basic key configuration.
  return $config_form;
}
