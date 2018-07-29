<?php

/**
 * Implements hook_views_handlers().
 */
function govcms_ckan_views_views_handlers() {
  return [
    'info' => [
      'path' => drupal_get_path('module', 'govcms_ckan_views'),
    ],
    'handlers' => [],
  ];
}

/**
 * Implements hook_views_plugins().
 */
function govcms_ckan_views_views_plugins() {
  $base = ['ckan_dataset'];

  return [
    'module' => 'govcms_ckan_views',
    'query' => [
      'ckan_datastore' => [
        'title' => t('CKAN API Query'),
        'help' => t('Query handler to make API queries directly to a CKAN datastore'),
        'handler' => 'views_plugin_query_ckan_datastore',
      ],
    ],
    'style' => [
      'ckan_bubbles' => [
        'title' => t('CKAN Bubbles'),
        'help' => t('Display result data with CKAN bubbles plugins'),
        'handler' => 'views_plugin_style_ckan_bubbles',
        'help-topic' => 'style-ckan-bubbles',
        'uses row plugin' => FALSE,
        'uses row class' => FALSE,
        'uses options' => TRUE,
        'uses fields' => TRUE,
        'type' => 'ckan',
        'base' => $base,
      ],
      'ckan_visualisation' => [
        'title' => t('CKAN Visualisation'),
        'help' => t('Display result data with CKAN visualisation plugins'),
        'handler' => 'views_plugin_style_ckan_visualisation',
        'help-topic' => 'style-ckan-visualisation',
        'uses row plugin' => FALSE,
        'uses row class' => FALSE,
        'uses options' => TRUE,
        'uses fields' => FALSE,
        'type' => 'ckan',
        'base' => $base,
      ],
    ],
  ];
}

/**
 * Implements hook_views_data().
 */
function govcms_ckan_views_views_data() {
  $data = array();

  // Instruct views to use the defined query class for CKAN dataset views.
  $data['ckan_dataset']['table']['group'] = t('CKAN Datastore');
  $data['ckan_dataset']['table']['base'] = array(
    'title' => t('CKAN datastore'),
    'help' => t('Query CKAN datastore API'),
    'query class' => 'ckan_datastore',
  );

  $data['ckan_dataset']['ckan_resource_id'] = array(
    'title' => t('CKAN Resources'),
    'help' => t('Include selected resources in the search'),
    'filter' => array(
      'handler' => 'views_handler_filter_resource_id',
    ),
  );

  // Add a filter to the view. This field will allow the view creator to specify
  // which API field the user will be submitting so we don't need to create a
  // specific filter for each type of field.
  //
  // @TODO: Potentially need to create generic "string", "int" filters if the
  // API supports different operators for each field.
  $data['ckan_dataset']['ckan_field'] = array(
    'title' => t('CKAN Field'),
    'help' => t('A field that is present in the datastore'),
    'filter' => array(
      'handler' => 'views_handler_filter_ckan_field',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_ckan_field',
    ),
  );

  $data['ckan_dataset']['ckan_label_key'] = array(
    'title' => t('CKAN Label Key'),
    'help' => t('Field used as labels(represented on axis)'),
    'filter' => array(
      'handler' => 'views_handler_filter_ckan_label_key',
    ),
  );

  $data['ckan_dataset']['ckan_visualisation'] = array(
    'title' => t('CKAN Visualisation'),
    'help' => t('Chart type of the CKAN dataset'),
    'filter' => array(
      'handler' => 'views_handler_filter_ckan_visualisation',
    ),
  );

  $data['ckan_dataset']['ckan_subdata'] = array(
    'title' => t('CKAN Subdata'),
    'help' => t('Choose which rows to display for each piece of information within a dataset'),
    'filter' => array(
      'handler' => 'views_handler_filter_ckan_subdata',
    ),
  );

  $data['ckan_dataset']['ckan_resource'] = array(
    'title' => t('CKAN Resource'),
    'help' => t('Provides a CKAN resource field.'),
    'field' => array(
      'handler' => 'views_handler_field_ckan_resource',
    ),
  );

  return $data;
}
