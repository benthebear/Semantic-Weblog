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
	semanticweblog_vocab_is_set();
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
    '#title' => t('Second Member'),
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
  global $base_url;
  semanticweblog_vocab_is_set();
  $collapse_state = semanticweblog_api_check_associations(arg(2));
  $form = array();
  $form["semanticweblog_ajax_add_association"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => $collapse_state,
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
    '#title' => t('Second Member'),
    '#default_value' => '',
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the second Member of your Association'),
  );
  
  $form["semanticweblog_ajax_add_association"]["role2-ajax"]["#suffix"] = "<div id='result'/><input type='button' onclick='semanticweblog_send_assoc(\"".$base_url."\");' value='".t('Create Association on the fly')."' />";
  return $form;
}


// Form, Validation and Submit of the Bulk Form

/**
 * Form for the Bulk Association Creation Form
 *
 * @return array $form A Drupal Form Array
 */
function semanticweblog_bulk_add_association() {
	semanticweblog_vocab_is_set();
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

function semanticweblog_form_association_admin_filter(){ 
	semanticweblog_vocab_is_set();
	$advanced_collapsed = TRUE;
  if(arg(3)!="" && arg(4) == ""){
    $term = semanticweblog_load_all_terms(array(arg(3)));
    $term = $term[arg(3)];
  }
  if (arg(3)!="" && arg(4) != "" && arg(5) != ""){
  	$advanced_collapsed = FALSE;
  	$term_advanced = semanticweblog_load_all_terms(array(arg(3)));
  	$terms_advanced["firstmember"] = $term_advanced[arg(3)];
  	$atid_advanced = arg(4);
  	$term_advanced = semanticweblog_load_all_terms(array(arg(5)));
  	$terms_advanced["secondmember"] = $term_advanced[arg(5)];
  }
  if(arg(3)==""){
    $term = "";
  }
  
	$form = array();
  $form["semanticweblog_form_association_admin_filter"] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => t("Filter Associations"),
    );
  $form["semanticweblog_form_association_admin_filter"]["term"] = array(
    '#type' => 'textfield',
    '#title' => t('Term'),
    '#default_value' => $term,
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    '#description' => t('Enter a term to search for all its associations'),
  );
  $form["semanticweblog_form_association_admin_filter"]["advanced"] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => $advanced_collapsed,
    '#title' => t("advanced"),
    );
	$form["semanticweblog_form_association_admin_filter"]["advanced"]["role1-filter"] = array(
    '#type' => 'textfield',
    '#title' => t('First Member'),
    '#default_value' => $terms_advanced["firstmember"],
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the first Member of your Association'),
  );

  $types = semanticweblog_get_all_types();
  $options["0"] = t("Any Associationtype");
	foreach ($types as $type) {	  
		$options[$type["atid"]] = $type["name"];
  }
	$form["semanticweblog_form_association_admin_filter"]["advanced"]['atid-filter'] = array(
    '#type' => 'select',
    '#title' => t('Association Type'),
    '#default_value' => $atid_advanced,
    '#size' => 1,
    //'#description' => t('How are your Topics related?'),
    '#options' => $options,
  );
	$form["semanticweblog_form_association_admin_filter"]["advanced"]["role2-filter"] = array(
    '#type' => 'textfield',
    '#title' => t('Second Member'),
    '#default_value' => $terms_advanced["secondmember"],
    '#size' => 30,
    '#maxlength' => 512,
    '#autocomplete_path' => 'taxonomy/autocomplete/' . variable_get('semanticweblog_lexicon', ""),
    //'#description' => t('Enter a name for the second Member of your Association'),
  );
	$form["semanticweblog_form_association_admin_filter"]["submit"] = array(
    '#type' => 'submit',
    '#value' => t('Filter Associations'),
  );
	
	if ($term or $terms_advanced) {
	$form["semanticweblog_form_association_admin_filter"]['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
      '#submit' => array('semanticweblog_form_association_admin_filter_reset'),
    );
	}
	return $form;
}

function semanticweblog_form_association_admin_filter_submit($form, &$form_state){
	if ($form_state["values"]["term"] != "") {
		$term = semanticweblog_term2tid($form_state["values"]["term"]);   
    drupal_goto("admin/content/association/" . $term );
    return;
  } else{
   		$tid1 = semanticweblog_term2tid($form_state["values"]["role1-filter"]);
   		$tid2 = semanticweblog_term2tid($form_state["values"]["role2-filter"]);
    	if ($tid1 == "") { $tid1 = "0"; }
    	if ($tid2 == "") { $tid2 = "0"; }
    	$atid = $form_state["values"]["atid-filter"];
  		drupal_goto("admin/content/association/" . $tid1 . "/" . $atid . "/" . $tid2);
  	}
  
  
}

function semanticweblog_form_association_admin_filter_reset($form, &$form_state){
	drupal_goto("admin/content/association/");
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
 * Checks wether an vocabulary was set for use in the forms
 */
function semanticweblog_vocab_is_set() {
	$vid = variable_get('semanticweblog_lexicon', "");
	if ($vid != "") {
	} else {
		drupal_set_message(l(t('You have to set a vocabulary to be your lexicon first.'), 'admin/settings/semanticweblog'), 'warning', FALSE);
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

/**
* Loads all, really all raw Associations
*
* @return array $raw_associations A raw association array
*/
function semanticweblog_load_all_associations() {
  $result = db_query("SELECT * FROM {semanticweblog_associations}");  
  $raw_associations = semanticweblog_association_result2items($result);
  return $raw_associations;
}

/**
* Loads all all raw Associations, paginated
*
* @return array $raw_associations A raw association array
*/
function semanticweblog_load_all_associations_paged() {
  $limit = variable_get('semanticweblog_admin_associations_pager', "10");
  $result = pager_query("SELECT * FROM {semanticweblog_associations}", $limit, $element = 0, $count_query = NULL);
  $raw_associations = semanticweblog_association_result2items($result);
  return $raw_associations;
}

function semanticweblog_load_filtered_associations(){
  $term1 = arg(3);
  $type = arg(4);
  $term2 = arg(5);
  
  $query = "SELECT * FROM {semanticweblog_associations}";  
  
  // Create the where clause
  $first = true;
  $where = "";
  if($term1 != "" and $type=="" and $term2==""){
    $where = " WHERE ";
    $where .= "member1tid = %d OR member2tid = %d";
    $args[] = $term1;
    $args[] = $term1;
    $first = false;
  }elseif($term1 !="" or $type !="" or $term2 !=""){
    $where = " WHERE ";
    if($term1 !="0"){
      $where .= "member1tid = %d";
      $first = false;
      $args[] = $term1;
    }
    if($type !="0"){
       if (!$first){
         $where .= " AND ";     
       }
      $where .= "atid = %d";
      $first = false;
      $args[] = $type;
    }
    if($term2 !="0"){
       if (!$first){
         $where .= " AND ";     
       }
      $where .= "member2tid = %d";
      $first = false;
      $args[] = $term2;
    }
    
  }
  if(!$first){
    $query = $query.$where;
  }
  $result = db_query($query, $args);
  
  $raw_associations = semanticweblog_association_result2items($result);
  return $raw_associations;
}

/**
* Loads the term-IDs and Names of a given Set of Terms
* 
* @param array $tids An numeric Array containing Term-IDs
* @return array $rich_terms A associative Array containing all IDs and Names of the given Terms.
*/
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
    $rich_terms = array();
    while ($item = db_fetch_array($result)) {     
      $rich_terms[$item['tid']] = $item["name"];
    }
  }
  return $rich_terms;
}

/**
* A helper function, converting a DB-Result-Resource into an raw_association array
*
* $raw_associations[$aid]["aid"] = ID of the Association
* $raw_associations[$aid]["uid"] = ID of the user who created it
* $raw_associations[$aid]["member1tid"] = ID of the first (downward) Term
* $raw_associations[$aid]["atid"] = ID of the Association Type
* $raw_associations[$aid]["member2tid"] = ID of the second (upward) Term
* $raw_associations[$aid]["created"] = TIMESTAMP of the date the association was created
*
* @param db-result-resource $result A Database Result-Resource
* @return array $raw_associations A raw association array
*/
function semanticweblog_association_result2items($result){
  $raw_associations = array();
  while ($item = db_fetch_array($result)) {
    //dprint_r($item);
    $raw_associations[$item['aid']] = $item;
  }
  return $raw_associations;
}

/**
 * Get (all/some) Associations
 *
 * @param bool $paged Wether the Request should be paginated
 * @return array $all All or some Associations
 */
function semanaticweblog_get_associations($display) {
 
  // Get all oer some Associations
  $assocs = array();
  switch($display){
    case "filter":
      $raw_associations = semanticweblog_load_filtered_associations();
      break;
    case "paged":
      $raw_associations = semanticweblog_load_all_associations_paged();
      break;
    case "all":    
      $raw_associations = semanticweblog_load_all_associations();
      break;    
  }
  
  return semanticweblog_types_and_associations2array($raw_associations);
}


/**
* Delivers complete Information on a set of Assiciations.
*
* This function expects an array, which is the result of 
* an db_fetch_array on an SQL Select on the semanticweblog_associations Query.
*
* $raw_associations[$aid]["aid"] = ID of the Association
* $raw_associations[$aid]["uid"] = ID of the user who created it
* $raw_associations[$aid]["member1tid"] = ID of the first (downward) Term
* $raw_associations[$aid]["atid"] = ID of the Association Type
* $raw_associations[$aid]["member2tid"] = ID of the second (upward) Term
* $raw_associations[$aid]["created"] = TIMESTAMP of the date the association was created
*
* The result is a more complex Array, containing the Names of the Terms and Association Types
*
* // The type of this Association
* $rich_associations[$aid]["type"]["atid"] = ID of the Association Type
* $rich_associations[$aid]["type"]["name"] = Name of the Association Type
* $rich_associations[$aid]["type"]["sentence"] = Sentence Version of the Association Type
* $rich_associations[$aid]["type"]["role1"] = First Role of the Association Type
* $rich_associations[$aid]["type"]["role2"] = Second of the Association Type
* // The first member of this Associattion
* $rich_associations[$aid]["member1"]["tid"] = ID of the first (downward) Term
* $rich_associations[$aid]["member1"]["name"] = Name of the first (downward) Term
* $rich_associations[$aid]["member1"]["role"] = Role of the first (downward) Term
* // The second member of this Association
* $rich_associations[$aid]["member2"]["tid"] = ID of the second (upward) Term
* $rich_associations[$aid]["member2"]["name"] = Name of the second (upward) Term
* $rich_associations[$aid]["member2"]["role"] = Role of the second (upward) Term
*
* @param array $associations An Array, representing the rows in the association table
* @return array $result A more complex Array, containing Names of all Terms and Association types
*
*/
function semanticweblog_types_and_associations2array($raw_associations){  
  // First load all Association types
  $types = semanticweblog_get_all_types();
   
  // Create the (still empty) result array
  $rich_associations = array();
  
  if(count($raw_associations)>0){
    // Gather all needed Termids
    foreach ($raw_associations as $assoc) {
      $tids[] = $assoc["member1tid"];
      $tids[] = $assoc["member2tid"];
    }
    // Get all need neede Term-Names by their IDs
    $terms = semanticweblog_load_all_terms($tids);
  
    // Build the rich_association array
 
    foreach ($raw_associations as $myass) {
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
      // Build the second member of this Association
      $myass["member2"]["tid"] = $myass["member2tid"];
      $myass["member2"]["name"] = $terms[$myass["member2tid"]];
      $myass["member2"]["role"] = $myass["type"]["role2"];
      // Add this association to the results
      $rich_associations[$myass["aid"]] = $myass;
    }
  }
  // sort the association array according to name of member1
	$sort_assocs = variable_get('semanticweblog_admin_associations_sorting', "none");
	if ($sort_assocs != "none") {
		usort($rich_associations, "semanticweblog_sort_assocs");
	}
	return $rich_associations;
}

/**
 * A helper function for sorting
 */
function semanticweblog_sort_assocs($a, $b) {
	$sortorder = variable_get('semanticweblog_admin_associations_sorting', "none");
	switch ($sortorder) {
		case "alpha_asc":
			return strcmp($a['member1']['name'], $b['member1']['name']);
			break;
		
		case "alpha_desc":
			return strcmp($b['member1']['name'], $a['member1']['name']);
			break;
	}
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
