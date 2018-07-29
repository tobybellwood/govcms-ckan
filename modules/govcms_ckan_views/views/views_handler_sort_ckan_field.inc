<?php
/**
 * @file
 * Class views_handler_sort_ckan_field.
 *
 * Generic sort filter for CKAN visualisation views.
 * This will allow you to sort by the year.
 */

class views_handler_sort_ckan_field extends views_handler_sort {
  function option_definition() {
    $options = parent::option_definition();

    $options['ckan_field_sort'] = '';

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $view = $this->view;
    $filters = $view->display_handler->get_option('filters');

    $ckan_field_filter_labels = $this->get_ckan_field_filter_labels($filters);

    $form['ckan_field_sort'] = array(
      '#type' => 'radios',
      '#title' => t('Sortable CKAN Fields'),
      '#options' => $ckan_field_filter_labels,
      '#description' => t('The CKAN field you wish to sort.'),
      '#default_value' => $this->options['ckan_field_sort'],
    );
  }

  /**
   * Save the submitted form values.
   *
   * @param array $form
   *   The form.
   * @param array $form_state
   *   The submitted form values.
   */
  function sort_submit(&$form, &$form_state) {
    parent::sort_submit($form, $form_state);

    $this->options['ckan_field_sort'] = $form_state['values']['options']['ckan_field_sort'];
  }

  /**
   * Called to add the sort to a query.
   */
  function query() {
    $this->ensure_my_table();
  }

  /**
   * Get the list of CKAN fields used for filtering.
   *
   * @param array $filters
   *   The list of view filters.
   *
   * @return array
   *   An array of filtered values.
   */
  function get_ckan_field_filters($filters) {
    $ckan_field_filters = array();

    foreach ($filters as $key => $value) {
      if (preg_match('/^ckan_field/', $key)) {
        $ckan_field_filters[$key] = $value;
      }
    }

    return $ckan_field_filters;
  }

  /**
   * Get the labels for the list of CKAN fields.
   *
   * @param array $filters
   *   The list of view filters.
   *
   * @return array
   *   An array of field labels.
   */
  function get_ckan_field_filter_labels($filters) {
    $filters = $this->get_ckan_field_filters($filters);
    $filter_labels = array();

    foreach ($filters as $value) {
      $id = $value['id'];
      $label = $value['expose']['label'] . ' (' . $value['ckan_field_name'] . ')';

      $filter_labels[$id] = $label;
    }

    return $filter_labels;
  }
}
