<?
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

function semanticweblog_term_get_own_associations($termid){
  
}

function semanticweblog_term_get_foreign_associations($termid){
  
}

function semanticweblog_term_get_all_associations($termid){  
  $result = db_query("SELECT * FROM {semanticweblog_associations} WHERE member1tid = %d or member2tid = %d", $termid, $termid);
  $associations = semanticweblog_association_result2items($result);
  //dprint_r($associations);
  return semanticweblog_types_and_associations2array($associations);
}