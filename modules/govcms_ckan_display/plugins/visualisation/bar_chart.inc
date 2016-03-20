<?php
/**
 * @file
 * Bar chart visualisation.
 */

$plugin = array(
  'title' => t('Bar chart visualisation'),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_display_bar_chart_view() {
  return 'This will be a renderable array. We will also load up our js using a \'#attached\' property';
}

/**
 * Configure form callback.
 */
function govcms_ckan_display_bar_chart_configure($plugin, $form, $form_state, $config) {
  // Get default key elements.
  $config_form = _govcms_ckan_media_field_widget_get_key_elements($form, $form_state, $config);

  $config_form['stacked'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is this this stacked.'),
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

  return $config_form;
}
