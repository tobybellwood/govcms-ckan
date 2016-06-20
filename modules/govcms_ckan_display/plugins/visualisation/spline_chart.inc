<?php
/**
 * @file
 * Line chart visualisation.
 */

$plugin = array(
  'title' => t('Spline chart'),
  'settings' => array(
    'rotated' => 'false',
    'area' => 0,
    'show_labels' => 0,
    'hide_points' => 0,
    'grid' => NULL,
    'grid_lines' => array('lines' => array()),
    'palette_override' => NULL,
    'show_title' => 1,
    'x_label' => NULL,
    'y_label' => NULL,
    'column_overrides' => array(),
    'area_opacity' => 20,
    'label_settings' => array(
      'overrides' => NULL,
    ),
    'axis_settings' => array(
      'x_tick_rotate' => 0,
      'x_tick_count' => NULL,
      'y_tick_count' => NULL,
      'x_tick_cull' => NULL,
    ),
    'ckan_filters' => array(
      'search' => NULL,
      'filters' => NULL,
    ),
  ),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_display_spline_chart_view($file, $display, $config) {
  $element = array();
  $chart_class = 'ckan-spline-chart';
  $ckan_search = govcms_ckan_get_config_value($config, 'ckan_filters/search');
  $ckan_filters = govcms_ckan_get_config_value($config, 'ckan_filters/filters');
  $response = govcms_ckan_client_request_records($file->resource_id, $ckan_search, $ckan_filters);

  // If failure, provide error message.
  if ($response->valid === FALSE) {
    $element['#theme'] = 'ckan_empty_visualisation';
  }
  else {

    // Setup our configuration.
    $keys = array_filter($config['keys']);
    $column_overrides = govcms_ckan_display_parse_column_overrides(govcms_ckan_get_config_value($config, 'column_overrides'));
    $label_replacements = govcms_ckan_string_to_array(govcms_ckan_get_config_value($config, 'label_settings/overrides'));

    // Attributes for the table.
    $attributes = array(
      'class' => array('ckan-chart', $chart_class),

      // Entity settings.
      'data-type' => (govcms_ckan_get_config_value($config, 'area') == 1 ? 'area-spline' : 'spline'),
      'data-rotated' => govcms_ckan_get_config_value($config, 'rotated'),
      'data-labels' => (govcms_ckan_get_config_value($config, 'show_labels') == 1 ? 'true' : 'false'),
      'data-hidePoints' => (govcms_ckan_get_config_value($config, 'hide_points') == 1 ? 'true' : 'false'),
      'data-grid' => govcms_ckan_get_config_value($config, 'grid'),
      'data-showTitle' => (govcms_ckan_get_config_value($config, 'show_title') == 1 ? 'true' : 'false'),
      'data-title' => $file->filename,
      'data-xLabel' => govcms_ckan_get_config_value($config, 'x_label'),
      'data-yLabel' => govcms_ckan_get_config_value($config, 'y_label'),
      'data-xTickRotate' => (govcms_ckan_get_config_value($config, 'axis_settings/x_tick_rotate') == 1 ? '90' : '0'),
      'data-xTickCount' => govcms_ckan_get_config_value($config, 'axis_settings/x_tick_count'),
      'data-yTickCount' => govcms_ckan_get_config_value($config, 'axis_settings/y_tick_count'),
      'data-xTickCull' => govcms_ckan_get_config_value($config, 'axis_settings/x_tick_cull'),
      'data-gridLines' => govcms_ckan_json_encode_attribute(govcms_ckan_get_config_value($config, 'grid_lines/lines', array())),

      // Display settings.
      'data-palette' => (!empty($config['palette_override']) ? $config['palette_override'] : $config['palette']),
      'data-exportWidth' => govcms_ckan_get_config_value($config, 'export_width'),
      'data-exportHeight' => govcms_ckan_get_config_value($config, 'export_height'),
      'data-xAxisLabelPos' => govcms_ckan_get_config_value($config, 'x_axis_label_position'),
      'data-yAxisLabelPos' => govcms_ckan_get_config_value($config, 'y_axis_label_position'),
      'data-xTickCentered' => (govcms_ckan_get_config_value($config, 'x_axis_tick_centered', 1) == 1 ? 'true' : 'false'),
    );

    // Parse the data.
    $parser = govcms_ckan_dataset_parser($response->data);
    $parser
      ->setKeys($keys)
      ->setLabelKey($config['labels'])
      ->setHeaderSource($config['x_axis_grouping'])
      ->setLabelReplacements($label_replacements)
      ->setTableAttributes($attributes)
      ->setColumnAttributes($column_overrides);

    // If splitting on the value of a key.
    if ($config['split']) {
      $parser->setGroupKey($config['split']);
    }

    // Return the parsed tables in a wrapper.
    $element = array(
      '#theme' => 'ckan_display_table_wrapper',
      '#tables' => $parser->parse(),
      '#show_titles' => !empty($config['split']),
    );

    // Add the JS to the tables.
    govcms_ckan_display_attach_charts($element, '.' . $chart_class);
  }

  return $element;
}

/**
 * Configure form callback.
 */
function govcms_ckan_display_spline_chart_configure($plugin, $form, &$form_state, $config) {
  // Get default key elements.
  $config_form = govcms_ckan_media_visualisation_default_key_config($form, $form_state, $config);

  $config_form['rotated'] = array(
    '#type' => 'select',
    '#title' => t('Orientation'),
    '#default_value' => govcms_ckan_get_config_value($config, 'rotated'),
    '#options' => array(
      'false' => t('Vertical'),
      'true' => t('Horizontal'),
    ),
  );

  $config_form['area'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable area'),
    '#default_value' => govcms_ckan_get_config_value($config, 'area'),
  );

  $area_opacity_options = array(10, 20, 30, 40, 50, 60, 70, 80, 90, 100);
  $config_form['area_opacity'] = array(
    '#type' => 'select',
    '#title' => t('Area Opacity'),
    '#default_value' => govcms_ckan_get_config_value($config, 'area_opacity'),
    '#options' => array_combine($area_opacity_options, $area_opacity_options),
    '#states' => array(
      'visible' => array(
        // This field is deeply nested so resorting to a fairly ugly selector
        // if selector is not found the in the edge case that the lang or delta
        // is different field will fallback to being visible anyway.
        ':input#edit-ckan-visualisation-und-0-config-visualisation-config-area' => array('checked' => TRUE),
      ),
    ),
  );

  $config_form['show_labels'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable data labels'),
    '#default_value' => $config['show_labels'],
  );

  $config_form['hide_points'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable data points'),
    '#default_value' => govcms_ckan_get_config_value($config, 'hide_points'),
  );

  $config_form['palette_override'] = array(
    '#title' => t('Palette override'),
    '#type' => 'textfield',
    '#default_value' => govcms_ckan_get_config_value($config, 'palette_override'),
    '#description' => t('Palette is a comma separated list of hex values. If not set, default palette is applied.'),
  );

  $config_form['grid'] = array(
    '#type' => 'select',
    '#title' => t('Enable grid'),
    '#default_value' => govcms_ckan_get_config_value($config, 'grid'),
    '#empty_option' => t('None'),
    '#options' => array(
      'x' => t('X Lines'),
      'y' => t('Y Lines'),
      'xy' => t('Both X and Y lines'),
    ),
  );

  // Add axis settings.
  $axis_config_form = govcms_ckan_media_visualisation_default_axis_config($form, $form_state, $config);
  $config_form = array_merge($config_form, $axis_config_form);

  return $config_form;
}
