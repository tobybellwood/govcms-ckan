<?php

/**
 * Class views_plugin_query_ckan_datastore
 *
 * Views query plugin that encapsulates calls to the API defined
 * in the CKAN module.
 */

class views_plugin_query_ckan_datastore extends views_plugin_query {

  /**
   * A list of filters to apply to the API request.
   * @var array
   */
  protected $filters = [];

  /**
   * A list of resource IDs we will be querying.
   * @var array
   */
  protected $resources = [];

  /**
   * The result key to sort by.
   *
   * @var string
   */
  protected $sortKey = '';

  /**
   * Prepare the filters to be sent to the endpoint.
   *
   * @TODO this may need to change if the API supports different operators.
   */
  private function prepare_filters() {
    return empty($this->filters) ? FALSE : json_encode($this->filters);
  }

  /**
   * Add a filter to the query.
   *
   * @param string $field
   *   A field to query on.
   * @param string $value
   *   The value to query for.
   *
   * @TODO: Investigate CKAN API as it may support operators to be parsed in
   * via the JSON string.
   */
  public function add_filter($field, $value) {
    if (empty($this->filters[$field])) {
      $this->filters[$field] = [];
    }

    $this->filters[$field][] = $value;
  }

  /**
   * Resource ID/s should be set via the Resource ID filter.
   *
   * @param $id
   *   A CKAN resource ID.
   */
  public function add_resource($id, $index) {
    $this->resources[] = array(
      'id' => $id,
      'index' => $index,
    );
  }

  private function get_resources() {
    $rids = [];
    foreach ($this->resources as $resource) {
      $rids[$resource['index']][] = $resource['id'];
    }
    return $rids;
  }

  /**
   * Get the query string to send to the CKAN API.
   *
   * @param string $id
   *   A valid resource ID.
   *
   * @return array
   *   A query array that can be passed to url().
   *
   * @see url()
   */
  private function get_query($id) {
    $opts = ['id' => $id];

    $query = $this->prepare_filters();

    if (!empty($query)) {
      $opts['filters'] = $query;
    }

    return $opts;
  }

  /**
   * Send an API request to the views defined CKAN resource.
   *
   * A view can have multiple resources defined. This will allow the user
   * to query multiple datasets and merge the results into one graph.
   */
  public function execute(&$view) {
    $view->result = [];
    foreach ($this->get_resources() as $index => $ids) {
      foreach ($ids as $id) {
        $client = govcms_ckan_client();
        $response = $client->get('action/datastore_search', $this->get_query($id));

        $data = $response->data;
        if (!empty($data->records)) {
          $view->result[$index][$id]['data'] = $this->prepare_result($data->records, $id, $view);
          $view->result[$index][$id]['id'] = $id;
        }
      }
    }
  }

  /**
   * Prepare a result set to pass to the theme.
   */
  public function prepare_result(&$data, &$id, &$view) {
    $data = $this->prepare_labels($data, $id, $view);

    $data = $this->sort_data($data, $view);

    return $data;
  }

  /**
   * Replace the labels with custom labels from the Dataset Type labels field.
   *
   * A dataset type has an optional field to override the existing labels.
   * This function uses the Data sets resources vocabulary & Label key, as defined in
   * the view > Page: Style options.
   *
   * @param $id
   *   A CKAN resource ID.
   */
  public function prepare_labels(&$data, &$id, &$view) {
    // $style_options will be used to get the Data sets resources terms & Label key.
    $style_options = $view->style_options;

    if (isset($style_options)) {
      // Get the Data sets resources terms & Label key.
      $vid = $style_options['data_structure']['resources_taxonomy'];
      $terms = $this->get_taxonomy_terms($vid);
      $label_key = $style_options['ckan_visualisation']['visualisation_config']['labels'];

      // Get the key|value pairs for label replacement from the term's field_labels field.
      foreach ($terms as $term) {
        if (strpos($term->description, $id) !== false) {
          $field_labels = field_get_items('taxonomy_term', $term, 'field_labels');
          // Convert key|value string into an associative array.
          // A taxonomy term can only contain one instance of field_labels.
          $labels_arr = govcms_ckan_string_to_array($field_labels[0]['value']);
        }
      }

      // Find the old label(s) and replace with the new label(s).
      if (isset($labels_arr)) {
        foreach ($labels_arr as $key => $label) {
          foreach ($data as $data_value) {
            if (isset($data_value->$label_key) && $data_value->$label_key == $key) {
              $data_value->$label_key = $label;
            }
          }
        }
      }
    }

    return $data;
  }

  /**
   * Return a set of terms for a particular taxonomy vocabulary.
   *
   * @param string $vid
   *   The vid of the vocabulary.
   * @return array $terms
   *   The set of terms of the chosen vocabulary.
   */
  public function get_taxonomy_terms($vid) {
    $vocabulary = taxonomy_vocabulary_load($vid);
    $terms = entity_load('taxonomy_term', FALSE, array('vid' => $vocabulary->vid));
    return $terms;
  }

  /**
   * Sort the passed in data.
   *
   * @param array $data
   *   The data to be sorted.
   * @param array $view
   *   The view containing all the information we need.
   *
   * @return array
   *   The sorted data.
   */
  public function sort_data(&$data, $view) {
    $sorts = $view->display_handler->get_option('sorts');

    if (empty($sorts)) {
      return $data;
    }

    $exposed_raw_input = $view->exposed_raw_input;
    $sort_by = $exposed_raw_input['sort_by'];

    $filters = $view->display_handler->get_option('filters');

    if (empty($sorts[$sort_by]['ckan_field_sort'])) {
      return $data;
    }

    $sort_field = $sorts[$sort_by]['ckan_field_sort'];

    $this->sortKey = $filters[$sort_field]['ckan_field_name'];

    if (strtolower($exposed_raw_input['sort_order']) == 'asc') {
      uasort($data, array($this, 'ckan_data_compare'));
    }
    else {
      uasort($data, array($this, 'ckan_data_reverse_compare'));
    }

    return $data;
  }

  /**
   * Custom sort comparison callback for ckan data.
   *
   * @param array $a
   *   The first item to be compared.
   * @param array $b
   *   The second item to be compared.
   *
   * @return int
   *   The result of the comparison.
   */
  private function ckan_data_compare($a, $b) {
    return strcasecmp($a->{$this->sortKey}, $b->{$this->sortKey});
  }

  /**
   * Custom reverse sort comparison callback for ckan data.
   *
   * @param array $a
   *   The first item to be compared.
   * @param array $b
   *   The second item to be compared.
   *
   * @return int
   *   The result of the comparison.
   */
  private function ckan_data_reverse_compare($a, $b) {
    return strcasecmp($b->{$this->sortKey}, $a->{$this->sortKey});
  }

}
