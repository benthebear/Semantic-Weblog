<?php
// $Id$

/* Author: Benjamin Birkenhake <benjamin@birkenhake.org>*/

/**
* @file:
* This file contains all functions which are meant to be used outside of the module in other modules or themes
*
* @todo: Add function for the genealogy (parents, grantparents, children, grandchildren) for a given termid
*
*
*/

/**
* History
*
* 2009 12 30 [ben]: Created this file
*/

/**
* This function get's all Associations in which a give Term is the first member (upwards)
*
* @param termid integer The ID of the given Term
* @return associations array An Array with all necessary Information of the Associations.
* @see semanticweblog_term_get_foreign_associations(), semanticweblog_term_get_all_associations()
*/
function semanticweblog_api_term_get_own_associations($termid){
  $result = db_query("SELECT * FROM {semanticweblog_associations} WHERE member1tid = %d ", $termid);
  $raw_associations = semanticweblog_association_result2items($result);
  return semanticweblog_types_and_associations2array($raw_associations);
}

/**
* This function get's all Associations in which a give Term is the second member (downwards)
*
* @param termid integer The ID of the given Term
* @return associations array An Array with all necessary Information of the Associations.
* @see semanticweblog_term_get_own_associations(), semanticweblog_term_get_all_associations()
*/
function semanticweblog_api_term_get_foreign_associations($termid){
  $result = db_query("SELECT * FROM {semanticweblog_associations} WHERE member2tid = %d", $termid);
  $raw_associations = semanticweblog_association_result2items($result);
  return semanticweblog_types_and_associations2array($raw_associations);
}

/**
* This function get's all Associations in which a give Term is a member
*
* @param termid integer The ID of the given Term
* @return associations array An Array with all necessary Information of the Associations.
* @see semanticweblog_term_get_own_associations(), semanticweblog_term_get_foreign_associations()
*/
function semanticweblog_api_term_get_all_associations($termid){  
  $result = db_query("SELECT * FROM {semanticweblog_associations} WHERE member1tid = %d or member2tid = %d", $termid, $termid);
  $raw_associations = semanticweblog_association_result2items($result);
  return semanticweblog_types_and_associations2array($raw_associations);
}


/**
* This function simply checks if any associations exists for the given term
*
* @param termid integer The ID of the given Term
* @return bool
*/
function semanticweblog_api_check_associations($termid){  
  $result = db_query("SELECT aid FROM {semanticweblog_associations} WHERE member1tid = %d or member2tid = %d LIMIT 1", $termid, $termid);
  $data = db_fetch_array($result);
 	if ($data) { return TRUE; }
 	else { return FALSE; }
}