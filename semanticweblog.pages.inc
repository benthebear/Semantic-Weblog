<?php
// $Id$

/**
  * @file
  * All Pages of the Module
  */

function semanticweblog_register_vocabulary($vid="all", $letter="A") {
  // Build the Query
  $sql = "SELECT * FROM {term_data} WHERE ";
  if ($vid!="all") {
    $wheres[] = " (vid=%d) ";
  }
  if (preg_match("/[a-zA-Z]/", $letter)) {
    $wheres[] = " name LIKE '%s%' ";
  }
  if ($letter == "-") {
    $letters = semanticweblog_get_alphabet_array();
    foreach ($letters as $myletter) {
      $wheres[] = " name NOT LIKE '" . $myletter . "%' ";
    }
  }
  $sql .= implode(" AND ", $wheres);
  $sql .= " ORDER BY name ";

  // Choose which parameter have to be given to db_query()
  if ($vid!="all" and preg_match("/[a-zA-Z]/", $letter)) {
    $result = db_query($sql, $vid, $letter);
  }
  elseif ($vid!="all" and $letter == "-") {
    $result = db_query($sql, $vid);
  }
  elseif (preg_match("/[a-zA-Z]/", $letter)) {
    $result = db_query($sql, $letter);
  }
  elseif ($vid=="all" and $letter == "-") {
    $result = db_query($sql);
  }


  while ($term = db_fetch_object($result)) {
    $terms[] = $term;
  }
  $vocabulary = taxonomy_vocabulary_load($vid);
  //return var_dump($vocabulary);
  if (isset($vocabulary->name)) {
    drupal_set_title('Register "' . $vocabulary->name . '"');
  }
  $output .= theme("semanticweblog_register_alphabet", $vid, $letter);
  $output .= theme("semanticweblog_register_vocabulary", $terms, $letter);
  return  $output;
}

function semanticweblog_get_alphabet_array() {
  $letters[] = "A";
  $letters[] = "B";
  $letters[] = "C";
  $letters[] = "D";
  $letters[] = "E";
  $letters[] = "F";
  $letters[] = "G";
  $letters[] = "H";
  $letters[] = "I";
  $letters[] = "J";
  $letters[] = "K";
  $letters[] = "L";
  $letters[] = "M";
  $letters[] = "N";
  $letters[] = "O";
  $letters[] = "P";
  $letters[] = "Q";
  $letters[] = "R";
  $letters[] = "S";
  $letters[] = "T";
  $letters[] = "U";
  $letters[] = "V";
  $letters[] = "W";
  $letters[] = "X";
  $letters[] = "Y";
  $letters[] = "Z";
  return $letters;

}

/**
 * This function theme a register Page
 *
 * @param array $terms
 * @param string $letter
 */
function theme_semanticweblog_register_vocabulary($terms, $letter="A") {
  $output .= "";
  if (is_array($terms)) {
    $output .= "<ul>";
    foreach ($terms as $term) {
      $output .="<li>" . l($term->name, "taxonomy/term/" . $term->tid, array('absolute' => TRUE)) . "</li>";
    }
    $output .= "</ul>";
  }
  else{
    $output .= t("There are no Terms starting with that Letter in this vocabulary.");
  }
  return $output;
}

function theme_semanticweblog_register_alphabet($vid, $letter = "A") {
  $output = "";
  $letters = semanticweblog_get_alphabet_array();
  $output .= '<p class="sw_alphabet_index">';

  foreach ($letters as $thisletter) {
    if ($letter == $thisletter) {
      $active = "active";
    }
    else {
      $active="";
    }
    $myletter = $thisletter;

    $output .= '<span class="letter ' . $active . '">' . l($thisletter, 'register/' . $vid . '/' . drupal_strtolower($thisletter), array('absolute' => TRUE)) . '</span> ' . "\n";
  }
  $output .= '<span class="letter">' . l("#", "register/" . $vid . "/-", array('absolute' => TRUE)) . '</span> ' . "\n";
  $output .= "</p>";
  return $output;

}

// Pages to administer Association Types

function semanticweblog_administer_associationtypes() {
  $output = "";
  $types = semanticweblog_load_all_types();
  foreach ($types as $type) {
    $rows[] = array($type["atid"], $type["name"], $type["role1"], $type["role2"], l("edit", "admin/content/associationtype/" . $type["atid"] . "/edit") . " | " . l("delete", "admin/content/associationtype/" . $type["atid"] . "/delete") );
  }
  $output .=  theme_table(array("No.", "Name", "First Role", "Second Role", "Actions"), $rows);
  $output .= drupal_get_form("semanticweblog_manage_association_types");
  return $output;
}

function semanticweblog_administer_associationtypes_edit($atid) {
  $output = "";
  $output .= "Edit Associationtype No." . $atid;
  $output .= drupal_get_form("semanticweblog_edit_association_type");
  return $output;
}

function semanticweblog_administer_associationtypes_delete($atid) {
  $output = "";
  $atid = arg(3);
  $ats = semanticweblog_load_all_types();
  $myat = $ats[$atid];
  $output .= "Are you really sure, you want to delete Association Type No. " . $atid . " '" . $myat['name'] . "'";
  $output .= drupal_get_form("semanticweblog_delete_association_type");
  return $output;
}

// Pages to administer Associations

function semanticweblog_administer_associations() {
  drupal_add_css(drupal_get_path('module', 'semanticweblog')  . '/semanticweblog.css');
  $output = "";
  // Association Creation Form
  $output .= drupal_get_form("semanticweblog_basic_add_association");

  // All Associations
  $assocs = semanaticweblog_get_all_associations();
  //dprint_r($assocs);
  foreach ($assocs as $assoc) {
    $rows[] = array(
      $assoc["member1"]["role"],
      l($assoc["member1"]["name"], "taxonomy/term/" . $assoc["member1"]["tid"]),
      $assoc["type"]["name"],
      $assoc["member2"]["role"],
      l($assoc["member2"]["name"], "taxonomy/term/" . $assoc["member2"]["tid"]),
      l("delete", "admin/content/association/" . $assoc["aid"] . "/delete"),
    );
  }
  $output .=  theme_table(array("Role", "First Member", "Relation", "Role", "Second Member", "Actions"), $rows);

  return $output;
}

function semanticweblog_add_add_association($term1, $atid, $term2){
  $termid1 = semanticweblog_term2tid($term1);
  $termid2 = semanticweblog_term2tid($term2);
  $types = semanticweblog_load_all_types();
  
  if(is_numeric($termid1) and is_numeric($termid2) and is_numeric($atid)) {
     print "Association  Created: " . $term1 . "(" . $termid1 . ") – " . $types[$atid]["name"] . " (" . $atid . ")" . " – " . $term2 . "(" . $termid2. ")";
    semanticweblog_insert_association($termid1, $atid, $termid2);
  }else{
    print  t('Error').": ";
    if(!is_numeric($termid1)){
      print t("The first Term does not exists. ");
    }
     if(!is_numeric($termid2)){
        print t("The second Term does not exists. ");
      }
   
  }
 
  die;
}