<?php
/**
 * @file
 * Defines the configuration widget field.
 */

/**
 * Implements hook_field_info().
 */
function govcms_ckan_media_field_info() {
  return array(
    'govcms_ckan_media_config' => array(
      'label' => t('CKAN display config'),
      'description' => t('This field stores the configuration for the display of a CKAN graph.'),
      'default_widget' => 'govcms_ckan_media_config_widget',
      'default_formatter' => 'govcms_ckan_media_config_formatter',
      // Not working yet, @see https://www.drupal.org/node/680910.
      'entity_types' => array('file'),
    ),
  );
}

/**
 * Implements hook_field_instance_settings_form().
 */
function govcms_ckan_media_field_instance_settings_form($field, $instance) {
  // Placeholder for if and when we want to add additional field config.
  // Return settings form.
  return array();
}

/**
 * Implements hook_field_is_empty().
 */
function govcms_ckan_media_field_is_empty($item, $field) {
  return empty($item['config']) && (string) $item['config'] !== '0';
}

/**
 * Implements hook_field_widget_info().
 */
function govcms_ckan_media_field_widget_info() {
  return array(
    'govcms_ckan_media_config_widget' => array(
      'label' => t('Graph configuration'),
      'field types' => array('govcms_ckan_media_config'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_validate().
 */
function govcms_ckan_media_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  // Placeholder for if and when we want to add additional validation.
  foreach ($items as $delta => $item) {
    // Validate each delta.
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function govcms_ckan_media_field_formatter_info() {
  return array(
    'govcms_ckan_media_config_formatter' => array(
      'label' => t('Hidden'),
      'field types' => array('govcms_ckan_media_config'),
    ),
  );
}

/**
 * Implements hook_field_presave().
 *
 * Serialise configuration on field save.
 */
function govcms_ckan_media_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  if ($field['type'] == 'govcms_ckan_media_config') {
    foreach ($items as $delta => $item) {
      // Only serialise if item['value'] is an array.
      if (is_array($item['config'])) {
        $items[$delta]['config'] = serialize($item['config']);
      }
    }
  }
}

/**
 * Implements hook_field_presave().
 *
 * Unserialise configuration on field load.
 */
function govcms_ckan_media_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  if ($field['type'] == 'govcms_ckan_media_config') {
    foreach ($entities as $id => $entity) {
      foreach ($items[$id] as $delta => $item) {
        // Only unserialise if item['value'] is a string.
        if (is_string($item['config'])) {
          $items[$id][$delta]['config'] = unserialize($item['config']);
        }
      }
    }
  }
}

/**
 * Implements hook_field_widget_form().
 */
function govcms_ckan_media_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  // If not dealing with a config widget, exit here.
  // TODO: Also check we only dealing with a file_entity type.
  if ($instance['widget']['type'] != 'govcms_ckan_media_config_widget') {
    return $element;
  }

  // Things we use often.
  $field_name = $field['field_name'];
  $file = $form_state['file'];

  // Get the config values.
  $config = array();
  // As much of this is ajax loaded we check input first for current state.
  if (isset($form_state['input'][$field_name])) {
    $config = $form_state['input'][$field_name][$langcode][$delta]['config'];
  }
  elseif (isset($file->{$field_name}[$langcode][$delta])) {
    // Fallback to saved values.
    $config = $file->{$field_name}[$langcode][$delta]['config'];
  }

  // Base widget is a fieldset tree.
  $widget = $element + array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );

  // Ajax wrapper for including the visualisation settings.
  $vis_wrapper_id = 'visualisation-settings-' . $delta;

  // The visualisation selector. This will ajax load the configuration form for
  // the selected visualisation.
  $widget['visualisation'] = array(
    '#title' => t('Visualisation'),
    '#type' => 'select',
    '#default_value' => isset($config['visualisation']) ? $config['visualisation'] : NULL,
    '#required' => ($form_state['build_info']['form_id'] != 'field_ui_field_edit_form'),
    '#options' => _govcms_ckan_media_field_widget_visualisation_options(),
    '#ajax' => array(
      'callback' => 'govcms_ckan_media_field_widget_visualisation_form',
      'wrapper' => $vis_wrapper_id,
    ),
  );

  // The wrapper element for visualisation configuration.
  $widget['visualisation_config'] = array(
    '#type' => 'container',
    '#attributes' => array('id' => $vis_wrapper_id),
  );

  // If config says there is a configuration, load the form from the plugin.
  if (!empty($config['visualisation'])) {
    $config_form = govcms_ckan_get_plugin_config($config['visualisation'], $form, $form_state, $config['visualisation_config']);
    $widget['visualisation_config'] = array_merge($widget['visualisation_config'], $config_form);
  }

  $element['config'] = $widget;
  return $element;
}

/**
 * Ajax callback for changing visualisations.
 */
function govcms_ckan_media_field_widget_visualisation_form($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $parents = $form_state['triggering_element']['#parents'];
  array_pop($parents);
  $el = drupal_array_get_nested_value($form, $parents);
  return $el['visualisation_config'];
}

/**
 * Implements hook_field_widget_error().
 */
function govcms_ckan_media_field_widget_error($element, $error, $form, &$form_state) {
  switch ($error['error']) {
    case 'validate_url':
      // Placeholder for validating.
      break;
  }
  //form_error($error_element, $error['message']);
}

/************
 * Helpers.
 ************/

/**
 * Get all the available visualisations as select options.
 *
 * @return array
 *   A key/value array of vis name/human name.
 */
function _govcms_ckan_media_field_widget_visualisation_options() {
  $plugins = govcms_ckan_get_all_plugins();
  $options = array();
  foreach ($plugins as $plugin) {
    $options[$plugin['name']] = $plugin['title'];
  }
  return $options;
}

/**
 * This helper provides the common elements for selecting the keys.
 *
 * @param array $form
 *   Form array for the parent form.
 * @param array $form_state
 *   Current form state array from the parent form.
 * @param array $config
 *   The current configuration values.
 *
 * @return array
 *   Form structure for new elements.
 */
function _govcms_ckan_media_field_widget_get_key_elements($form, $form_state, $config = array()) {
  $form = array();

  // We need a resource_id, however this file might not be saved yet, if it is
  // we can get it from the entity, otherwise, need to re-parse the embed code.
  if (isset($form_state['file'])) {
    $resource_id = $form_state['file']->resource_id;
  }
  else {
    $embed_code = $form_state['storage']['embed_code'];
    $handler = new MediaInternetGovCmsCkanHandler($embed_code);
    $uri = $handler->parse($embed_code);
    $resource_id = $handler->parseIdFromUri($uri);
  }

  // Make a request to obtain the fields.
  $client = govcms_ckan_client();
  $result = $client->get('action/datastore_search', array('id' => $resource_id));

  // If something went wrong with request we cannot continue.
  if ($result->valid === FALSE || !isset($result->data->fields)) {
    drupal_set_message(t('Unable to retrieve data for this package'), 'error');
    return $form;
  }

  // Loop over fields to build some lists for options.
  $options = array();
  foreach ($result->data->fields as $field) {
    // Ignore the ID field.
    if ($field->id == '_id') {
      continue;
    }
    // Group by field type.
    $options[$field->type][$field->id] = $field->id;
    // All fields.
    $options['all'][$field->id] = $field->id;
  }

  // Build the elements.
  $form['keys'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Keys'),
    '#options' => $options['numeric'],
    '#default_value' => $config['keys'],
    '#multiple' => TRUE,
    '#description' => t('What data to display in the visualisation.'),
  );

  $form['labels'] = array(
    '#type' => 'select',
    '#title' => t('Labels'),
    '#options' => $options['text'],
    '#default_value' => $config['labels'],
    '#description' => t('What field contains the labels.'),
  );

  $form['split'] = array(
    '#type' => 'select',
    '#title' => t('Split field'),
    '#options' => $options['all'],
    '#default_value' => $config['split'],
    '#empty_option' => t('- Select -'),
    '#description' => t('Optionally spit into multiple visualisations based on the value of this key.'),
  );

  // This indicates to future parsers that these elements have been included.
  $form['key_defaults'] = array(
    '#type' => 'hidden',
    '#value' => 1,
  );

  return $form;
}
