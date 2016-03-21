<?php
/**
 * @file
 * Line chart visualisation.
 */

$plugin = array(
  'title' => t('Line chart'),
  'settings' => array(
    'rotated' => 'false',
    'area' => 0,
    'show_labels' => 0,
    'x_label' => NULL,
    'y_label' => NULL,
    'grid' => NULL,
  ),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_display_line_chart_view($file, $display, $config) {
  $element = array();
  $chart_class = 'ckan-line-chart';
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
      'data-type' => ($config['area'] == 1 ? 'area' : 'line'),
      'data-rotated' => $config['rotated'],
      'data-labels' => ($config['show_labels'] == 1 ? 'true' : 'false'),
      'data-grid' => $config['grid'],
      'data-xLabel' => $config['x_label'],
      'data-yLabel' => $config['y_label'],
    );

    // Parse the data.
    $parser = govcms_ckan_dataset_parser();
    $element = $parser
      ->setResult($response->data)
      ->setKeys($keys)
      ->setLabelKey($config['labels'])
      ->setGroupKey($config['split'])
      ->setHeaderSource($config['x_axis_grouping'])
      ->setTableAttributes($attributes)
      ->parse();

    // Add the JS to the tables.
    govcms_ckan_display_attach_charts($element, '.' . $chart_class);
  }

  return $element;
}

/**
 * Configure form callback.
 */
function govcms_ckan_display_line_chart_configure($plugin, $form, $form_state, $config) {
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

  $config_form['area'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable area'),
    '#default_value' => $config['area'],
  );

  $config_form['show_labels'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable data labels'),
    '#default_value' => $config['show_labels'],
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
