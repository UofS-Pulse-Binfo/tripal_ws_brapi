<?php
/**
 * This script uses the tripal_ws_brapi_testdata module to load Test Data.
 * This is the same action as going to https://[yourdrupalsite]/tripalwsbrapi/testdata
 */
include_once('../class/TripalWebServiceTestData');

// In this order, project info is required in phenotype, and organism
// info is require in stocks.
// Reverse order when remove command to prevent triggers from table constraints.
$source = [
	'project'    => ['main' => 'project-csv', 'prop' => 'projectprop-csv'],
	'project_relationship' => ['main' => 'project_relationship-csv'],
	'organism'   => ['main' => 'organism-csv'],
	'stock'      => ['main' => 'stock-csv', 'prop' => 'stockprop-csv'],
	'phenotype'  => ['main' => 'phenotype-csv' , 'prop' => 'phenotypeprop-csv', 'cvtermprop' => 'cvtermprop-csv'],
	'contact'    => ['main' => 'contact-csv', 'prop' => 'contactprop-csv'],
	'genotype'   => ['main' => 'genotype-csv'],
	'feature'    => ['main' => 'feature-csv', 'prop' => 'featureprop-csv', 'feature_relationship' => 'feature_relationship-csv', 'genotype_call' => 'genotype_call-csv'],
	'featuremap' => ['main' => 'featuremap-csv', 'prop' => 'featuremapprop-csv', 'featuremap_organism' => 'featuremap_organism-csv', 'featurepos' => 'featurepos-csv'],
	'cvterm'     => ['main' => 'cvterm-csv'],
];
$testdata = new TripalWebServiceTestData($source);
$testdata->doRequest('load');
