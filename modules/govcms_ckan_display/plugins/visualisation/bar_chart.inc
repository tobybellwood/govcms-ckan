<?php
/**
 * @file
 * Bar chart visualisation.
 */

$plugin = array(
  'title' => t('Bar chart'),
  'settings' => array(
    'rotated' => 'false',
    'stacked' => 0,
    'x_label' => NULL,
    'y_label' => NULL,
    'grid' => NULL,
  ),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_display_bar_chart_view($file, $display, $config) {
  $element = array();
  $chart_class = 'ckan-bar-chart';
  $response = govcms_ckan_client_request_records($file->resource_id);

  // If failure, provide error message.
  if ($response->valid === FALSE) {
    $element['#theme'] = 'ckan_empty_visualisation';
  }
  else {

    // Setup our configuration.
    $keys = array_filter($config['keys']);

    // Attributes for the table.
    $attributes = array(
      'class' => array('ckan-chart', $chart_class),
      'data-type' => 'bar',

      // Entity settings.
      'data-rotated' => $config['rotated'],
      'data-stacked' => ($config['stacked'] == 1 ? 'true' : 'false'),
      'data-grid' => $config['grid'],
      'data-xLabel' => $config['x_label'],
      'data-yLabel' => $config['y_label'],

      // Display settings.
      'data-xTickCount' => $config['x_tick_count'],
      'data-yTickCount' => $config['y_tick_count'],
      'data-xTickCull' => $config['x_tick_cull'],
      'data-yTickCull' => $config['y_tick_cull'],
      'data-palette' => $config['palette'],
      'data-exportWidth' => $config['export_width'],
      'data-exportHeight' => $config['export_height'],
    );

    // Parse the data.
    $parser = govcms_ckan_dataset_parser();
    $parser
      ->setResult($response->data)
      ->setKeys($keys)
      ->setLabelKey($config['labels'])
      ->setHeaderSource($config['x_axis_grouping'])
      ->setTableAttributes($attributes)
      ->parse();

    // If splitting on the value of a key.
    if ($config['split']) {
      $parser->setGroupKey($config['split']);
    }

    // Parse the data into tables.
    $element = $parser->parse();

    // Add the JS to the tables.
    govcms_ckan_display_attach_charts($element, '.' . $chart_class);
  }

  return $element;
}

/**
 * Configure form callback.
 */
function govcms_ckan_display_bar_chart_configure($plugin, $form, $form_state, $config) {
  // Get default key elements.
  $config_form = govcms_ckan_media_visualisation_default_key_config($form, $form_state, $config);

  $config_form['rotated'] = array(
    '#type' => 'select',
    '#title' => t('Orientation'),
    '#default_value' => $config['rotated'],
    '#options' => array(
      'false' => t('Vertical'),
      'true' => t('Horizontal'),
    ),
  );

  $config_form['stacked'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is this this stacked'),
    '#default_value' => $config['stacked'],
  );

  $config_form['x_label'] = array(
    '#type' => 'textfield',
    '#title' => t('X axis label'),
    '#default_value' => $config['x_label'],
  );

  $config_form['y_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Y axis label'),
    '#default_value' => $config['y_label'],
  );

  $config_form['grid'] = array(
    '#type' => 'select',
    '#title' => t('Enable grid'),
    '#default_value' => $config['grid'],
    '#empty_option' => t('None'),
    '#options' => array(
      'x' => t('X Lines'),
      'y' => t('Y Lines'),
      'xy' => t('Both X and Y lines'),
    ),
  );

  return $config_form;
}
