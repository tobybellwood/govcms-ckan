<?php


class CkanDatasetViewsWizard extends ViewsUiBaseViewsWizard {

  /**
   * Override the build_form.
   *
   * Removes pager options from the wizard as these are not relevant.
   *
   * @TODO: Look at implementing pagination to this view type.
   */
  public function build_form($form, &$form_state) {
    $form = parent::build_form($form, $form_state);

    // Disable pager fields instead of unset them, otherwise will get undefined
    // index error.
    $form['displays']['page']['options']['items_per_page']['#disabled'] = TRUE;
    $form['displays']['page']['options']['items_per_page']['#default_value'] = '';
    $form['displays']['page']['options']['pager']['#disabled'] = TRUE;
    $form['displays']['page']['options']['pager']['#default_value'] = FALSE;

    return $form;
  }

  /**
   * Override the build_form_style so we can specify which plugins to show.
   *
   * Relies on Style plugins being defined with a 'ckan' type.
   *
   * @see parent::build_form_style
   * @see views_fetch_plugin_names
   * @see govcms_ckan_views_plugins
   */
  protected function build_form_style(&$form, &$form_state, $type) {
    $style_options = views_fetch_plugin_names('style', 'ckan', array($this->base_table));
    $style_form =& $form['displays'][$type]['options']['style'];
    $style_form['style_plugin']['#options'] = $style_options;
  }

  protected function default_display_options($form, $form_state) {
    $display_options = parent::default_display_options($form, $form_state);
    $display_options['style_plugin'] = 'ckan_visualisation';

    return $display_options;
  }

}
