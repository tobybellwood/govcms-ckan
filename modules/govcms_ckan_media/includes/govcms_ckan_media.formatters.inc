<?php
/**
 * @file
 * Formatters for CKAN datasets.
 */

/**
 * Implements hook_file_formatter_info().
 *
 * The basic file formatters for both preview and the file, the preview is
 * displayed as a static image and the file is a link to the public dataset.
 */
function govcms_ckan_media_file_formatter_info() {
  $formatters['govcms_ckan_media_public_link'] = array(
    'label' => t('CKAN public URL'),
    'file types' => array('ckan'),
    'view callback' => 'govcms_ckan_media_file_formatter_public_link_view',
    'mime types' => array('other/ckan'),
  );

  $formatters['govcms_ckan_media_image'] = array(
    'label' => t('CKAN Preview Image'),
    'file types' => array('ckan'),
    'default settings' => array(
      'image_style' => '',
    ),
    'view callback' => 'govcms_ckan_media_file_formatter_image_view',
    'mime types' => array('other/ckan'),
  );

  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 *
 * The formatter for the preview image.
 */
function govcms_ckan_media_file_formatter_image_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'ckan') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    // Create a thumbnail placeholder from a static image.
    return array(
      '#theme' => 'image',
      '#path' => $wrapper->getThumbnailPath(),
      '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
    );
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 *
 * The formatter for the link to the public dataset.
 */
function govcms_ckan_media_file_formatter_public_link_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme !== 'ckan') {
    return array();
  }

  // TODO: Render the file as a link to the public dataset, would prefer not to
  // do another request to get the package_id. Possible store in file_metadata
  // and use MediaReadOnlyStreamWrapper:getExternalUrl() to generate?
  return array('#markup' => filter_xss($file->filename));

}

/**
 * Implements hook_field_formatter_info().
 *
 * This is the formatter for the visualisation. The config field is rendered
 * using this formatter.
 */
function govcms_ckan_media_field_formatter_info() {
  return array(
    'govcms_ckan_media_config_formatter' => array(
      'label' => t('Visualisation'),
      'field types' => array('govcms_ckan_media_config'),
      'view callback' => 'govcms_ckan_media_field_formatter_view',
      'mime types' => array('other/ckan'),
      'file types' => array('ckan'),
      'settings' => array(
        'width' => 'auto',
        'height' => 'auto',
        'palette' => 'auto',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 *
 * The formatter for the config field that gets rendered as the visualisation.
 */
function govcms_ckan_media_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    case 'govcms_ckan_media_config_formatter':
      foreach ($items as $delta => $item) {

        // Load the config so we can include the correct renderer.
        $config = _govcms_ckan_media_get_visualisation_config($entity);
        if (isset($config['visualisation'])) {
          $element[$delta] = govcms_ckan_get_plugin_view($config['visualisation'], $entity, $display, $config['visualisation_config']);
        }

      }
      break;
  }

  return $element;
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 *
 * Generic formatter settings for the configuration.
 */
function govcms_ckan_media_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();

  $element['width'] = array(
    '#title' => t('Width'),
    '#type' => 'textfield',
    '#default_value' => $settings['width'],
    '#element_validate' => array('_govcms_ckan_media_visualisation_width_and_height'),
    '#description' => 'A pixel value (without unit) or "auto"',
  );
  $element['height'] = array(
    '#title' => t('Height'),
    '#type' => 'textfield',
    '#default_value' => $settings['height'],
    '#element_validate' => array('_govcms_ckan_media_visualisation_width_and_height'),
    '#description' => 'A pixel value (without unit) or "auto"',
  );

  // TODO: Add validator & summary.
  $element['palette'] = array(
    '#title' => t('Palette override'),
    '#type' => 'textfield',
    '#default_value' => $settings['palette'],
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function govcms_ckan_media_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = t('Width: @width, Height: @height', array(
    '@width'     => $settings['width'],
    '@height'  => $settings['height'],
  ));
  return $summary;
}

/**
 * Validation for width and height.
 */
function _govcms_ckan_media_visualisation_width_and_height($element, &$form_state, $form) {
  // Check if the value is a number or auto.
  if (!empty($element['#value']) && $element['#value'] != 'auto' && !is_numeric($element['#value'])) {
    form_error($element, t("The value entered for @dimension is invalid. Please insert a unitless integer for pixels or 'auto'", array('@dimension' => $element['#title'])));
  }
}