<?php
/**
 * @file
 * Set the default CKAN visualisation.
 */

/**
 * Set "Table" as the default CKAN visualisation.
 */
function govcms_ckan_update_7001(&$sandbox) {
  variable_set('govcms_ckan_default_visualisation', 'table');

  return t('Default CKAN visualisation now set to "Table".');
}
