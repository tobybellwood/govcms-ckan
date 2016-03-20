<?php
/**
 * @file
 * Pie chart visualisation.
 */

$plugin = array(
  'title' => t('Pie chart visualisation'),
);

/**
 * Returns a renderable array that represents the block content.
 */
function govcms_ckan_display_pie_chart_view() {
  return 'This will be a renderable array. We will also load up our js using a \'#attached\' property';
}

/**
 * Configure form callback.
 */
function govcms_ckan_display_pie_chart_configure($plugin, $form, $form_state, $config) {
  $config_form = array();

  $config_form['show_labels'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show piechart labels.'),
    '#default_value' => $config['show_labels'],
  );

  return $config_form;
}