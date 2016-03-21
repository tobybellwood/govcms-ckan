<?php
/**
 * @file
 * Theme functionality for govcms_ckan_display.
 */

/**
 * Implements hook_preprocess_ckan_empty_visualisation().
 */
function template_preprocess_ckan_empty_visualisation(&$variables) {
  // Set default empty from variable and fall back to default text.
  if (empty($variables['empty_text'])) {
    $default_text = t('Unable to load the data for this visualisation');
    $variables['empty_text'] = variable_get('govcms_ckan_display_empty_visualisation_text', $default_text);
  }
}
