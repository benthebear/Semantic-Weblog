<?php
// $Id$

/**
  * @file:
  * This file contains all non-hook and non-theme Functions with the purpose of managing Associaton Types
  */


function semanticweblog_load_all_types() {
  $result = db_query("SELECT * FROM {semanticweblog_types}");

  $items = array();
  while ($item = db_fetch_array($result)) {
    $items[$item['atid']] = $item;
  }
  return $items;
}

// The functions and forms for creating an Association Type

function semanticweblog_manage_association_types() {
  // Add an Assoctyoe
  $form["semanticweblog_manage_association_types"]["new_type_name"] = array('#type' => 'textfield',
    '#title' => t('Association Type Name'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["new_type_role1"] = array('#type' => 'textfield',
    '#title' => t('Association First Role'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the firste Role of the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["new_type_role2"] = array('#type' => 'textfield',
    '#title' => t('Association Second Role'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the second Role of the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["submit"] = array('#type' => 'submit',
    '#value' => t('Create new Association Type'),
  );
  return $form;
}

function semanticweblog_manage_association_types_validate($form, &$form_state) {
  if ($form_state['values']['new_type_name'] == '') {
    form_set_error('new_type_name', t('Your Association Type needs a Name.'));
  }
  if ($form_state['values']['new_type_role1'] == '') {
    form_set_error('new_type_role1', t('Your Association Type needs a first Role.'));
  }
  if ($form_state['values']['new_type_role2'] == '') {
    form_set_error('new_type_role2', t('Your Association Type needs a second Role.'));
  }
}

function semanticweblog_manage_association_types_submit($form, &$form_state) {
  $name = $form_state['values']['new_type_name'];
  $role1 = $form_state['values']['new_type_role1'];
  $role2 = $form_state['values']['new_type_role2'];
  semanticweblog_insert_associationtype($name, $role1, $role2);
  drupal_set_message(t('Your association type has been saved.'));
}

function semanticweblog_insert_associationtype($name, $role1, $role2) {
  db_query("INSERT INTO {semanticweblog_types} (name, role1, role2) VALUES ('%s', '%s', '%s')", $name, $role1,  $role2);
}





// The functions and forms for editing an Association type

function semanticweblog_edit_association_type() {
  // Edit an Assoctyoe
  $atid = arg(3);
  $ats = semanticweblog_load_all_types();
  $myat = $ats[$atid];
  $form["semanticweblog_manage_association_types"]["new_type_name"] = array('#type' => 'textfield',
    '#title' => t('Association Type Name'),
    '#default_value' => $myat["name"],
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["new_type_role1"] = array('#type' => 'textfield',
    '#title' => t('Association First Role'),
    '#default_value' => $myat["role1"],
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the firste Role of the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["new_type_role2"] = array('#type' => 'textfield',
    '#title' => t('Association Second Role'),
    '#default_value' => $myat["role2"],
    '#size' => 30,
    '#maxlength' => 512,
    '#description' => t('Enter a name for the second Role of the Association type'),
  );
  $form["semanticweblog_manage_association_types"]["atid"] = array('#type' => 'hidden',
    '#value' => $myat["atid"],
  );
  $form["semanticweblog_manage_association_types"]["submit"] = array('#type' => 'submit',
    '#value' => t('Edit this Association Type'),
  );
  return $form;
}

function semanticweblog_edit_association_type_validate($form, &$form_state) {
  if ($form_state['values']['new_type_name'] == '') {
    form_set_error('new_type_name', t('Your Association Type needs a Name.'));
  }
  if ($form_state['values']['new_type_role1'] == '') {
    form_set_error('new_type_role1', t('Your Association Type needs a first Role.'));
  }
  if ($form_state['values']['new_type_role2'] == '') {
    form_set_error('new_type_role2', t('Your Association Type needs a second Role.'));
  }
}

function semanticweblog_update_associationtype($name, $role1, $role2, $atid) {
  db_query("UPDATE {semanticweblog_types} SET name='%s', role1='%s', role2='%s' WHERE atid=%d", $name, $role1,  $role2, $atid);
}

function semanticweblog_edit_association_type_submit($form, &$form_state) {
  $atid = $form_state['values']['atid'];
  $name = $form_state['values']['new_type_name'];
  $role1 = $form_state['values']['new_type_role1'];
  $role2 = $form_state['values']['new_type_role2'];
  semanticweblog_update_associationtype($name, $role1, $role2, $atid);
  drupal_set_message(t('Your association type has been edited.'));
  drupal_goto("admin/content/associationtype");
}


// The functions and files for deleting an Association Type

function semanticweblog_delete_association_type() {  
  $atid = arg(3);
  $ats = semanticweblog_load_all_types();
  $myat = $ats[$atid];
  $form["semanticweblog_manage_association_types"]["atid"] = array('#type' => 'hidden',
    '#value' => $myat["atid"],
  );
  $form["semanticweblog_manage_association_types"]["submit"] = array('#type' => 'submit',
    '#value' => t('Delete this Association Type'),
  );
  return $form;
}

function semanticweblog_delete_associationtype($atid) {
  db_query("DELETE FROM {semanticweblog_types} WHERE atid=%d", $atid);
}

function semanticweblog_delete_association_type_submit($form, &$form_state) {
  $atid = $form_state['values']['atid'];
  semanticweblog_delete_associationtype($atid);
  drupal_set_message(t('Your association type has been deleted.'));
  drupal_goto("admin/content/associationtype");
}





