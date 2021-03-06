<?php

/**
 * @file
 * Page callback and submit handler for the default branch form.
 */

/**
 * Form constructor for the default branch form.
 */
function project_git_default_branch_form($form_state, $project) {
  $repo = $project->versioncontrol_project['repo'];
  $form['#repo'] = $repo;
  $form['#nid'] = $project->nid;

  // Add the radios widget for default branch selection.
  $form['default_branch'] = array(
    '#title' => t('The default repository branch'),
    '#description' => t('This branch will be checked out by default when a user clones, and shown by default under <a href="!version_control">Version control</a> instructions.', array('!version_control' => url('node/' . $project->nid . '/git-instructions'))),
    '#type' => 'radios',
    '#options' => array(),
  );

  // Make all branches available as options.
  $branches = $repo->loadBranches();
  foreach ($branches as $id => $branch) {
    $form['default_branch']['#options'][$id] = check_plain($branch->name);
  }
  uasort($form['default_branch']['#options'], '_project_git_default_branch_inverse_version_compare');

  // Make the current default branch the default selection.
  if ($repo->getDefaultBranch()) {
    $default_branch = $repo->loadBranches(NULL, array('name' => $repo->getDefaultBranch()));
    foreach ($default_branch as $id => $branch) {
      $form['default_branch']['#default_value'] = $id;
    }
  }

  // Add a submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form submission handler for project_git_default_branch_form().
 */
function project_git_default_branch_form_submit($form, &$form_state) {
  $repo = $form['#repo'];
  $default_branch = reset($repo->loadBranches(array($form_state['values']['default_branch'])));

  if ($default_branch) {
    try {
      $repo->setDefaultBranch($default_branch->name);
      $form_state['redirect'] = 'node/' . $form['#nid'];
      drupal_set_message(t('The default branch has been changed to %branch.', array('%branch' => $default_branch->name)));
    }
    catch (Exception $e) {
      drupal_set_message(filter_xss($e->getMessage()), 'error');
    }
  }
}

/**
 * Compares two branch names.
 *
 * Callback for uasort() within project_git_default_branch_form().
 *
 * @param $a
 *   The first branch name.
 * @param $b
 *   The secound branch name.
 *
 * @return
 *   The inverse of version_compare().
 */
function _project_git_default_branch_inverse_version_compare($a, $b) {
  return version_compare($b, $a);
}
