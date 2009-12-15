<?php
// $Id$

/**
  * @file:
  * This file contains all non-hook and non-theme Functions with the purpose of managing Associatons
  */

// Functions and Forms for Association Creation

function semanticweblog_block_add_association() {
  $output = "Let's rock!";
  $output .= drupal_get_form("semanticweblog_basic_add_association");
  return $output;
}

function semanticweblog_basic_add_association() {
  $form = array();
  $form["semanticweblog_basic_add_association"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t("Add an Association"),
    );
  $form["semanticweblog_basic_add_association"]["role1"] = array(
    '#type' => 'textfield',
    '#title' => t('First Member'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the first Member of your Association'),
  );

  $types = semanticweblog_load_all_types();
  foreach ($types as $type) {
    $options[$type["atid"]] = $type["name"];
  }

  $form["semanticweblog_basic_add_association"]['atid'] = array(
    '#type' => 'select',
    '#title' => t('Association Type'),
    '#default_value' => '',
    '#size' => 1,
    //'#description' => t('How are your Topics related?'),
    '#options' => $options,
  );

  $form["semanticweblog_basic_add_association"]["role2"] = array(
    '#type' => 'textfield',
    '#title' => t('Second Memeber'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the second Member of your Association'),
  );

  $form["semanticweblog_basic_add_association"]["submit"] = array(
    '#type' => 'submit',
    '#value' => t('Create new Association'),
  );
  return $form;
}

function semanticweblog_basic_add_association_validate($form, &$form_state) {
  if ($form_state['values']['atid'] == '') {
    form_set_error('new_type_name', t('Your Association needs an Association Type.'));
  }
  if ($form_state['values']['role1'] == '') {
    form_set_error('new_type_role1', t('Your Association needs a first Member.'));
  }
  if ($form_state['values']['role2'] == '') {
    form_set_error('new_type_role2', t('Your Association needs a second Member.'));
  }
}

function semanticweblog_term2tid($term) {
  $vid = variable_get('semanticweblog_lexicon', "");
  $typed_term = drupal_explode_tags($term);
  $possibilities = taxonomy_get_term_by_name($typed_term[0]);
  $typed_term_tid = NULL; // tid match, if any.
  foreach ($possibilities as $possibility) {
    if ($possibility->vid == $vid) {
        $typed_term_tid = $possibility->tid;
    }
  }
  return $typed_term_tid;
}

function semanticweblog_basic_add_association_submit($form, &$form_state) {
  $vid = variable_get('semanticweblog_lexicon', "");
  $tid1 = semanticweblog_term2tid($form_state["values"]["role1"]);
  $tid2 = semanticweblog_term2tid($form_state["values"]["role2"]);
  $atid = $form_state["values"]["atid"];
  semanticweblog_insert_association($tid1, $atid, $tid2);
  drupal_set_message(t('Your association has been saved.'));
  drupal_goto("admin/content/association");
}

function semanticweblog_insert_association($member1, $assoctype, $member2) {
  global $user;
  db_query("INSERT INTO {semanticweblog_associations} (uid, member1tid, atid, member2tid, created) VALUES (%d, %d, %d, %d, %d)", $user->uid, $member1, $assoctype, $member2, time());

}

// Functions for loading and displaying Associations.

function semanticweblog_load_all_associations() {
  $result = db_query("SELECT * FROM {semanticweblog_associations}");

  $items = array();
  while ($item = db_fetch_array($result)) {
    $items[$item['aid']] = $item;
  }
  return $items;
}

function semanticweblog_load_all_terms($tids) {
  $sql = "SELECT tid, name FROM {term_data} WHERE";
  foreach ($tids as $id) {
    $strings[] = " tid=" . $id . " ";
  }
  $where = implode(" or ", $strings);
  $sql .= $where;
  $result = db_query($sql);
  $items = array();
  while ($item = db_fetch_array($result)) {
    $items[$item['tid']] = $item["name"];
  }
  return $items;
}

function semanaticweblog_get_all_associations() {
  $types = semanticweblog_load_all_types();
  $assocs = semanticweblog_load_all_associations();
  foreach ($assocs as $assoc) {
    $tids[] = $assoc["member1tid"];
    $tids[] = $assoc["member2tid"];
  }
  $terms = semanticweblog_load_all_terms($tids);
  foreach ($assocs as $myass) {
    $myass["type"]["atid"] = $myass["member1tid"];
    $myass["type"]["name"] = $types[$myass["atid"]]["name"];
    $myass["type"]["role1"] = $types[$myass["atid"]]["role1"];
    $myass["type"]["role2"] = $types[$myass["atid"]]["role2"];

    $myass["member1"]["tid"] = $myass["member1tid"];
    $myass["member1"]["name"] = $terms[$myass["member1tid"]];
    $myass["member1"]["role"] = $myass["type"]["role1"];

    $myass["member2"]["tid"] = $myass["member2tid"];
    $myass["member2"]["name"] = $terms[$myass["member2tid"]];
    $myass["member2"]["role"] = $myass["type"]["role2"];


    $all[$myass["aid"]] = $myass;
  }

  return $all;
}






