<?php
// $Id$

/**
  * @file:
  * This file contains all non-hook and non-theme Functions with the purpose of managing Associatons
  *
  * @author Benjamin Birkenhake <benjamin@birkenhake.org>
  * @package semanticweblog
  * @copyright creative commons by-sa 
  *
  */


// Helper Functions

/**
 * Helper Function to get the TermId from a Term-String
 *
 * @param string $term The Name of the Term
 * @return integer $termid The ID of the Term or NULL if we didn't find it.
 */
function semanticweblog_term2tid($term) {
  $vid = variable_get('semanticweblog_lexicon', "");
  $typed_term = drupal_explode_tags($term);
  $possibilities = taxonomy_get_term_by_name($typed_term[0]);
  $termid = NULL; // tid match, if any.
  foreach ($possibilities as $possibility) {
    if ($possibility->vid == $vid) {
        $termid = $possibility->tid;
    }
  }
  return $termid;
}


// Functions and Forms for Association Creation

/**
 * Wraper Function for the Association Creation Block using Ajax 
 *
 * @return string $output The rendered Ajax Form
 */
function semanticweblog_block_add_association_ajax() {
  $output .= drupal_get_form("semanticweblog_ajax_add_association");
  return $output;
}

/**
 * Wraper Function for the Association Creation Block using Bulk
 *
 * @return string $output The rendered Bulkupload Form
 */
function semanticweblog_block_add_association_bulk() {
  $output .= drupal_get_form("semanticweblog_bulk_add_association");
  return $output;
}


// Form, Validation and Submit of the basic Association Form

/**
 * Form for the basic Association Creation Form
 *
 * @return array $form A Drupal Form Array
 */
function semanticweblog_basic_add_association() {
  $form = array();
  $form["semanticweblog_basic_add_association"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t("Add an Association"),
    );
  $form["semanticweblog_basic_add_association"]["role1-basic"] = array(
    '#type' => 'textfield',
    '#title' => t('First Member'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the first Member of your Association'),
  );

  $types = semanticweblog_get_all_types();
  foreach ($types as $type) {
    $options[$type["atid"]] = $type["name"];
  }

  $form["semanticweblog_basic_add_association"]['atid-basic'] = array(
    '#type' => 'select',
    '#title' => t('Association Type'),
    '#default_value' => '',
    '#size' => 1,
    //'#description' => t('How are your Topics related?'),
    '#options' => $options,
  );

  $form["semanticweblog_basic_add_association"]["role2-basic"] = array(
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

/**
 * Implementation of hook_validate for the basic form
 */
function semanticweblog_basic_add_association_validate($form, &$form_state) {
  if ($form_state['values']['atid-basic'] == '') {
    form_set_error('new_type_name', t('Your Association needs an Association Type.'));
  }
  if ($form_state['values']['role1-basic'] == '') {
    form_set_error('new_type_role1', t('Your Association needs a first Member.'));
  }
  if ($form_state['values']['role2-basic'] == '') {
    form_set_error('new_type_role2', t('Your Association needs a second Member.'));
  }
  $tid1 = semanticweblog_term2tid($form_state["values"]["role1-basic"]);
  $tid2 = semanticweblog_term2tid($form_state["values"]["role2-basic"]);
  if (semanticweblog_exists_association ($tid1, $form_state['values']['atid-basic'], $tid2)) {
    form_set_error('new_type_role2', t('Your Association already exists.'));
  }
}

/**
 * Implementation of hook_submit for the basic form
 */
function semanticweblog_basic_add_association_submit ($form, &$form_state) {
  $vid = variable_get('semanticweblog_lexicon', "");
  $tid1 = semanticweblog_term2tid($form_state["values"]["role1-basic"]);
  $tid2 = semanticweblog_term2tid($form_state["values"]["role2-basic"]);
  $atid = $form_state["values"]["atid-basic"];
  semanticweblog_insert_association($tid1, $atid, $tid2);
  drupal_set_message(t('Your association has been saved.'));
  drupal_goto("admin/content/association");
}


// Form of the Ajax Association Creation Block

/**
 * Form for the Ajax Association Creation Form
 *
 * @return array $form A Drupal Form Array
 */
function semanticweblog_ajax_add_association() {
  $form = array();
  $form["semanticweblog_ajax_add_association"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t("Add an Association"),
    );
  $form["semanticweblog_ajax_add_association"]["role1-ajax"] = array(
    '#type' => 'textfield',
    '#title' => t('First Member'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the first Member of your Association'),
  );

  $types = semanticweblog_get_all_types();
  foreach ($types as $type) {
    $options[$type["atid"]] = $type["name"];
  }

  $form["semanticweblog_ajax_add_association"]['atid-ajax'] = array(
    '#type' => 'select',
    '#title' => t('Association Type'),
    '#default_value' => '',
    '#size' => 1,
    //'#description' => t('How are your Topics related?'),
    '#options' => $options,
  );

  $form["semanticweblog_ajax_add_association"]["role2-ajax"] = array(
    '#type' => 'textfield',
    '#title' => t('Second Memeber'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the second Member of your Association'),
  );
  
  $form["semanticweblog_ajax_add_association"]["role2-ajax"]["#suffix"] = "<div id='result'/><input type='button' onclick='semanticweblog_send_assoc();' value='Creat Assoc on the fly'>";
  return $form;
}


// Form, Validation and Submit of the Bulk Form

/**
 * Form for the Bulk Association Creation Form
 *
 * @return array $form A Drupal Form Array
 */
function semanticweblog_bulk_add_association() {
  $form = array();
  $form["semanticweblog_bulk_add_association"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t("Add an Association"),
    );
  $form["semanticweblog_bulk_add_association"]["associations"] = array(
    '#type' => 'textarea',
    '#title' => t('Type your Associations'),
    '#default_value' => '',    
    '#rows' => 15,  
    '#description' => t('Enter your Associations. One line for each Association.') . " " . t("Use the Separator:") . " " . variable_get('semanticweblog_bulk_separator', "#") . " " . t("Example: Star Wars # is a # Movie"),
  );
  
  $form["semanticweblog_bulk_add_association"]["submit"] = array(
    '#type' => 'submit',
    '#value' => t('Create new Associations'),
  );

  return $form;
}


/**
 * Implementation of hook_validate for the basic form
 */
function semanticweblog_bulk_add_association_validate($form, &$form_state) {
  if($form_state['values']['associations']==""){
    form_set_error('associations', t("You haven't entered any association."));
  }
}

/**
 * Implementation of hook_submit for the bulk form
 *
 * This function parses a text-like List of Associations to be created
 */
function semanticweblog_bulk_add_association_submit ($form, &$form_state) {
  $associations = explode("\n",$form_state['values']['associations']);
  $counter_successes = 0;
  $counter_failures = 0;

  // Go on only if there is at least one line 
  if(is_array($associations) and count($associations) > 0){
    
    // Get the Separator 
    $sep = variable_get('semanticweblog_bulk_separator', "#");
    
    foreach($associations as $line){
      $parts = explode($sep, $line);      
      
      // Go on only if the line consists of three parts
      if(is_array($parts) and count($parts)==3){
        // Get TermIDs and Association Type ID
        $tid1 = semanticweblog_term2tid($parts[0]);
        $tid2 = semanticweblog_term2tid($parts[2]);
        $atid = semanticweblog_name2atid($parts[1]);
        
        // Go on only if all are numeric and not null
        if(is_numeric($tid1) and $tid1!=0 and is_numeric($atid) and $atid!=0 and is_numeric($tid2) and $tid2!=0){
          if(semanticweblog_insert_association($tid1,$atid,$tid2)){
            drupal_set_message(t("Added the following Association.") . " " . $line. " [".$tid1."][".$atid."][".$tid2."]");
            $counter_successes ++;
          }else{
            drupal_set_message(t("The followin Association already exists.") . " " . $line, "error");
            $counter_failures ++;
          }
        }else{         
          drupal_set_message(t("The following line could not be read.") . " " . $line . " [" . $tid1 . "][" . $atid . "][" . $tid2 . "]", "error");
          $counter_failures ++;
        }
      }else{        
        drupal_set_message(t("The following line could not be read.") . " " . $line . " [" . $tid1 . "][" . $atid . "][" . $tid2 . "]", "error");
        $counter_failures ++;
      }
    }
    
    // Set The Message on the Successes
    if($counter_successes > 0){
      drupal_set_message($counter_successes . " ". t('Associations have been created.'));
    }
    
    // Set The Message on the Failures
    if($counter_failures > 0){
      drupal_set_message($counter_failures . " ". t("Associations couldn't be created."), "error");
    }
    
  }else{
    drupal_set_message(t('No association has been created.'));
  }
  
  drupal_goto("admin/content/association");
}




/**
 * Checks wether an Association already exists
 *
 * @param integer $member1 The TermID of the first member
 * @param integer $assoctype The ID of the Association Type
 * @param integer $member2 The TermID of the second member
 * @return bool $bool Wether a given Association alread exists.
 */
function semanticweblog_exists_association ($member1, $assoctype, $member2) {
  $result = db_query("SELECT count(aid) FROM {semanticweblog_associations} WHERE member1tid = %d AND atid = %d AND member2tid = %d",$member1, $assoctype, $member2);
  $item = db_fetch_array($result);
  if ($item["count(aid)"] > 0) {
    return true;
  }else{
    return false;
  } 
}


/**
 * Inserts an new Association into the Database
 *
 * @param $member1 termID
 * @param $assoctype association-type-ID
 * @param $member2 termID
 * @return void
 */
function semanticweblog_insert_association ($member1, $assoctype, $member2) {
  if (!semanticweblog_exists_association ($member1, $assoctype, $member2)) {
    global $user;
    $result = db_query("INSERT INTO {semanticweblog_associations} (uid, member1tid, atid, member2tid, created) VALUES (%d, %d, %d, %d, %d)", $user->uid, $member1, $assoctype, $member2, time());
    //dprint_r($result);
    return $result;
  }else{
    return false;
  }
}

// Functions for loading and displaying Associations.

function semanticweblog_load_all_associations() {
  $result = db_query("SELECT * FROM {semanticweblog_associations}");  
  $items = semanticweblog_association_result2items($result);
  return $items;
}

function semanticweblog_load_all_associations_paged() {
  $limit = variable_get('semanticweblog_admin_associations_pager', "10");
  $result = pager_query("SELECT * FROM {semanticweblog_associations}", $limit, $element = 0, $count_query = NULL);
  $items = semanticweblog_association_result2items($result);
  return $items;
}

function semanticweblog_load_all_terms($tids) {
  $items = array();
  if(is_array($tids)){
    $sql = "SELECT tid, name FROM {term_data} WHERE";
    foreach ($tids as $id) {
      $strings[] = " tid=" . $id . " ";
    }
    $where = implode(" or ", $strings);
    $sql .= $where;
    $result = db_query($sql);
    $$items = array();
    while ($item = db_fetch_array($result)) {     
      $items[$item['tid']] = $item["name"];
    }
  }
  return $items;
}

function semanticweblog_association_result2items($result){
  $items = array();
  while ($item = db_fetch_array($result)) {
    //dprint_r($item);
    $items[$item['aid']] = $item;
  }
  return $items;
}

/**
 * Get (all/some) Associations
 *
 * @param bool $paged Wether the Request should be paginated
 * @return array $all All or some Associations
 */
function semanaticweblog_get_associations($paged) {
 
  
  // Get all oer some Associations
  $assocs = array();
  if($paged){
    $associations = semanticweblog_load_all_associations_paged();
  }else{
    $associations = semanticweblog_load_all_associations();
  }
  return semanticweblog_types_and_associations2array($associations);
  
}

function semanticweblog_types_and_associations2array($associations){  
  // First load all Association types
  $types = semanticweblog_get_all_types();
   
  // Create the (still empty) result array
  $result = array();
  
  if(count($associations)>0){
    // Gather all needed Termids
    foreach ($associations as $assoc) {
      $tids[] = $assoc["member1tid"];
      $tids[] = $assoc["member2tid"];
    }
    // Get all need neede Term-Names by their IDs
    $terms = semanticweblog_load_all_terms($tids);
  
    // Build the result array
 
    foreach ($associations as $myass) {
      // Build the type of this Association
      $myass["type"]["atid"] = $myass["member1tid"];
      $myass["type"]["name"] = $types[$myass["atid"]]["name"];
      $myass["type"]["sentence"] = $types[$myass["atid"]]["sentence"];
      $myass["type"]["role1"] = $types[$myass["atid"]]["role1"];
      $myass["type"]["role2"] = $types[$myass["atid"]]["role2"];
      // Build the first member of this Associattion
      $myass["member1"]["tid"] = $myass["member1tid"];
      $myass["member1"]["name"] = $terms[$myass["member1tid"]];
      $myass["member1"]["role"] = $myass["type"]["role1"];
      // Build the second memeber of this Association
      $myass["member2"]["tid"] = $myass["member2tid"];
      $myass["member2"]["name"] = $terms[$myass["member2tid"]];
      $myass["member2"]["role"] = $myass["type"]["role2"];
      // Add this association to the results
      $result[$myass["aid"]] = $myass;
    }
  }
  return $result;
}

// Functions for delete an association

/**
 * Delete an Association from the database
 *
 * @param integer $aid The ID of the association
 */
function semanticweblog_delete_association($aid){  
  db_query("DELETE FROM {semanticweblog_associations} WHERE aid = %d", $aid);
}

/**
 * The form for the confirm of the deletion of the association
 */
function semanticweblog_delete_association_form(){
  $aid = arg(3); 
  $form["semanticweblog_delete_association_form"]["aid"] = array('#type' => 'hidden',
    '#value' => $aid,
  );
  $form["semanticweblog_delete_association_form"]["submit"] = array('#type' => 'submit',
    '#value' => t('Delete this Association'),
  );
  return $form;
}

/**
 * Implementation of hook_submit for the Association delete form
 */
function semanticweblog_delete_association_form_submit($form, &$form_state){
  $aid = $form_state['values']['aid'];
  semanticweblog_delete_association($aid);
  drupal_set_message(t('Your association has been deleted.'));
  drupal_goto("admin/content/association");
}
