<?php

/**
 * Class views_plugin_style_ckan_visualisation
 *
 * Style plugin for views to integrate with CKAN visualisation plugins.
 */

class views_plugin_style_ckan_visualisation extends views_plugin_style {

  /**
   * @TODO: Update this so that the options form allow the user to choose which
   * visualisation to use when rendering the view.
   */

  /**
   * Ensure that uses_fields() always returns FALSE.
   *
   * This affects the Views UI by not allowing users to change how the display
   * behaves. All display options should be managed via code
   */
  public function uses_fields() {
    return FALSE;
  }

  /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['data_structure'] = array('default' => array());
    $options['ckan_visualisation'] = array('default' => array());

    return $options;
  }

  /**
   * Render the given style.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $data_structure = $this->options['data_structure'];
    $vis_wrapper_id = 'visualisation-settings-view';

    $form['data_structure'] = array(
      '#type' => 'fieldset',
      '#title' => t('CKAN Data Structure Source'),
      '#description' => t('<p>This section changes will affect the below <strong>CKAN Visualisation</strong> section. Save changes for this section first then come back to edit below <strong>CKAN Visualisation</strong> section.</p>'),
      '#tree' => TRUE,
    );

    // Get the list of data fields for configuration.
    $form['data_structure']['source'] = [
      '#type' => 'select',
      '#title' => t('Method for getting data structures'),
      '#description' => t('From taxonomy vocabulary or CKAN API'),
      '#default_value' => isset($data_structure['source']) ? $data_structure['source'] : 'vocabulary',
      '#options' => ['vocabulary' => 'Taxonomy Vocabulary', 'ckan_api' => 'CKAN API'],
    ];

    $form['data_structure']['fields_taxonomy'] = [
      '#type' => 'select',
      '#title' => t('CKAN fields vocabulary'),
      '#description' => t('Choose which vocabulary should be used'),
      '#default_value' => isset($data_structure['fields_taxonomy']) ? $data_structure['fields_taxonomy'] : '',
      '#options' => $this->get_vocabs(),
      '#empty_option' => '- Select -',
      '#states' => [
        'visible' => [
          ':input[name="style_options[data_structure][source]"]' => ['value' => 'vocabulary'],
        ],
      ],
    ];

    $form['data_structure']['resources_taxonomy'] = [
      '#type' => 'select',
      '#title' => t('Data sets resources vocabulary'),
      '#description' => t('Choose which vocabulary should be used'),
      '#default_value' => isset($data_structure['resources_taxonomy']) ? $data_structure['resources_taxonomy'] : '',
      '#options' => $this->get_vocabs(),
      '#empty_option' => '- Select -',
      '#states' => [
        'visible' => [
          ':input[name="style_options[data_structure][source]"]' => ['value' => 'ckan_api'],
        ],
      ],
    ];

    // Without data structure setting saved, we stop provide more configuration
    // form.
    // @todo Ajax callback is not working properly in here for some unknown reason.
    // So currently it's not using ajax. May need more investigating.
    if (empty($data_structure)) {
      return;
    }
    elseif ($data_structure['source'] === 'vocabulary' && empty($data_structure['fields_taxonomy'])) {
      return;
    }
    elseif ($this->options['data_structure']['source'] === 'ckan_api' && empty($this->options['data_structure']['resources_taxonomy'])) {
      return;
    }
    else {
      // Get the CKAN fields list.
      $source = array(
        'type' => $this->options['data_structure']['source'],
      );

      if ($this->options['data_structure']['source'] === 'vocabulary') {
        $source['vid'] = $this->options['data_structure']['fields_taxonomy'];
      }
      elseif ($this->options['data_structure']['source'] === 'ckan_api') {
        $source['vid'] = $this->options['data_structure']['resources_taxonomy'];
      }

      $form_state['ckan_fields'] = govcms_ckan_views_get_ckan_fields($source);
    }

    $form['ckan_visualisation'] = array(
      '#type' => 'fieldset',
      '#title' => t('CKAN Visualisation'),
      '#tree' => TRUE,
    );

    // The wrapper element for visualisation configuration.
    $form['ckan_visualisation']['visualisation_config'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => $vis_wrapper_id,
        'class' => array('govcms-ckan-view--settings-form'),
      ),
    );

    // Get the config.
    if (isset($this->options['ckan_visualisation']['visualisation_config'])) {
      $config = $this->options['ckan_visualisation']['visualisation_config'];
    }
    else {
      $config = array();
      $ckan_plugins = govcms_ckan_get_all_plugins();

      foreach ($ckan_plugins as $ckan_plugin) {
        $config = array_merge($ckan_plugin['settings'], $config);
      }
    }

    $form['ckan_visualisation']['visualisation_config']['visualisation'] = array(
      '#title' => t('Visualisation'),
      '#type' => 'select',
      '#default_value' => isset($config['visualisation']) ? $config['visualisation'] : NULL,
      '#required' => TRUE,
      '#options' => _govcms_ckan_media_field_widget_visualisation_options(),
      '#description' => t('How the data from this CKAN dataset will be presented <strong>by default</strong> when rendered on the page.'),
    );

    // Include default widgets.
    $widgets = array(
      'govcms_ckan_media_visualisation_default_key_config',
      'govcms_ckan_media_visualisation_default_axis_config',
      'govcms_ckan_media_visualisation_default_grid_config',
      'govcms_ckan_media_visualisation_default_chart_config',
    );
    $config_form = govcms_ckan_media_visualisation_include_form_widgets($form, $form_state, $config, $widgets);

    // Remove settings not suit for CKAN views.
    // @todo discuss and review all configurations to see which should be removed.
    // @todo Currently those custom settings for each chart type is not added. Add them here if need.
    unset($config_form['ckan_filters']);
    unset($config_form['column_overrides']);
    unset($config_form['label_settings']);

    $form['ckan_visualisation']['visualisation_config'] = array_merge($form['ckan_visualisation']['visualisation_config'], $config_form);

  }

  /**
   * Render the results of a request to the CKAN API.
   *
   * This method should be used to handle grouping result sets together a view
   * should always group the response from the CKAN API via the resource ID.
   *
   * eg.
   *    {RESOURCE_ID} => [ ... ]
   *    {RESOURCE_ID} => [ ... ]
   *
   * @see views_plugin_style::render
   * @see views_plugin_style::render_grouping_sets
   */
  public function render() {

    $element = [];

    // Get the config.
    $visualisation_config = $this->options['ckan_visualisation']['visualisation_config'];

    // Get the setting of merge multiple resources.
    $merge_resources = $this->view->filter['ckan_resource_id']->merge_resources;

    // Set the default visualisation chart type.
    $chart_type = $visualisation_config['visualisation'];

    // Get user selected chart type from exposed filter.
    if (!empty($this->view->visualisation)) {
      $chart_type = $this->view->visualisation;
    }

    foreach ($this->view->result as $index => $result) {

      // Merge multiple resources data in same data set type.
      if ($merge_resources) {
        $result_merged = array('ids' => array(), 'data' => array());
        foreach ($result as $row) {
          $result_merged['data'] = array_merge($result_merged['data'], $row['data']);
          $result_merged['ids'][] = $row['id'];
        }
        $result = array($result_merged);
      }

      foreach ($result as $row) {
        // Parse the data.
        $data = new stdClass();

        // Get title for chart from CKAN.
        $data->filename = '';

        if (!empty($row['ids']) && is_array($row['ids'])) {
          $client = govcms_ckan_client();
          $response = $client->get('action/resource_show', array('id' => reset($row['ids'])));
          $data->filename = isset($response->data->name) ? $response->data->name : '';
        }

        $data->data = new stdClass();
        $data->data->records = $row['data'];

        // Get user selected label key from exposed filter.
        if (!empty($this->view->label_key)) {
          $visualisation_config['labels'] = $this->view->label_key;
        }

        // Set the axis orientation.
        if (!empty($this->view->exposed_raw_input['label_axis'])) {
          if ($this->view->exposed_raw_input['label_axis'] == 'y') {
            $visualisation_config['rotated'] = 'true';
          }
        }

        // Get the rendered content.
        $element[] = govcms_ckan_get_plugin_view($chart_type, $data, ['settings' => []], $visualisation_config);
      }
    }

    return $element;
  }

  /**
   * Retrieve a list of vocabularies available for the site.
   *
   * @return array
   */
  public function get_vocabs() {
    $vocabs = taxonomy_get_vocabularies();
    $options = array();
    foreach ($vocabs as $vocab) {
      $options[$vocab->vid] = $vocab->name;
    }
    return $options;
  }

}
