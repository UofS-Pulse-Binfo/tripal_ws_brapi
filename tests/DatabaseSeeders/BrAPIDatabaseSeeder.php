<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;

class BrAPIDatabaseSeeder extends Seeder
{
    /**
     * Seeds the database with users.
     *
     * @return void
     */
    public function up()
    {

      // Organism
      $values = [
        [
          'abbreviation' => 'T. databasica',
          'genus' => 'Tripalus',
          'species' => 'databasica',
          'common_name' => 'Cultivated Tripal'
        ],
        [
          'abbreviation' => 'T. ferox',
          'genus' => 'Tripalus',
          'species' => 'ferox',
          'common_name' => 'Wild Tripal'
        ],
        [
          'abbreviation' => 'T. spideria',
          'genus' => 'Tripalus',
          'species' => 'spideria',
          'common_name' => 'Tripal WS'
        ],
        [
          'abbreviation' => 'B. specificus',
          'genus' => 'Brapus',
          'species' => 'specificus',
          'common_name' => 'Cultivated BrAPI'
        ],
        [
          'abbreviation' => 'B. tripalus',
          'genus' => 'Brapus',
          'species' => 'tripalus',
          'common_name' => 'BrAPI Tripal Hybrid'
        ]
      ];


      // Store in array all germplasm id corresponding to organism_id in chado.organism
      // after they have all been inserted. Later when, stocks are inserted will randomly
      // select a value from this id pool for the field stock.germplasm_id key.
      $all_organism = [];
      foreach($values as $organism) {
        $match = [
          'genus' => $organism['genus'],
          'species' => $organism['species'],
        ];
        $exists = chado_select_record('organism', ['organism_id'], $match);
        if ($exists) {
          $all_organism[] = $exists[0]->organism_id;
          chado_update_record('organism', $match, $organism);
        }
        else {
          $r = chado_insert_record('organism', $organism);
          $all_organism[] = $r['organism_id'];
        }
      }

      // Terms: value used in type_id when adding stock.
      //   We choose the type_ids of stock-based Tripal Content Types.
      $ids = db_query("SELECT type_id FROM {chado_bundle}
        WHERE data_table = 'stock' AND type_column = 'type_id'")->FetchCol();

      // Term used to indicate what term a value in prop table means.
      // Array below.
      $prop_stock_term = tripal_get_cvterm(array('name' => 'prop stock'));

      if (!$prop_stock_term) {
        // Term prop stock is not installed.
        $property = tripal_insert_cvterm(
          array('id' => 'test:' . 'prop stock', 'name' => 'prop stock', 'cv_name' =>'null')
        );

        $prop_stock_term_id = $property->cvterm_id;
      }
      else {
        // Prop stock term found in chado.cvterm
        $prop_stock_term_id = $prop_stock_term->cvterm_id;
      }

      $propvaltype = $prop_stock_term_id;

      $awesome = array(1 => 'awesome', 2 => 'not awesome', 3 => 'pretty');

      // Inserts 56 sample stocks.
      $mystocks= array(
      'lola', 'stella', 'luna', 'nala', 'izzy',
      'layla', 'lulu', 'ellie', 'piper', 'olivia',
      'king', 'max', 'owen', 'piper', 'ralph', 'rex',
      'scout', 'tesla', 'wyatt', 'ace', 'bailey',
      'bennet', 'chase', 'dexter', 'emmet', 'felix',
      'forrest', 'hudson', 'jack', 'emma', 'aria',
      'ava', 'mia', 'mila', 'chloe', 'zoe', 'abigail',
      'ariana', 'elena', 'eva', 'sarah', 'naomi', 'sadie',
      'liam', 'noah', 'lucas', 'leo', 'max',
      'dylan', 'landon', 'marcus', 'david', 'rowan',
      'cooper', 'gunner', 'tank');

      foreach($mystocks as $i => $s) {
        $organism = mt_rand(0, count($all_organism) - 1);
        $type_id = mt_rand(0, count($ids) - 1);

        $arr_stock = array(
          'organism_id' => $all_organism[ $organism ],
          'name' => $s,
          'uniquename' => 'uname-' . uniqid(),
          'type_id' => $ids[ $type_id ],
          'dbxref_id' => null,
          'description' => 'description ' . $s,
        );
        $stock = chado_insert_record('stock', $arr_stock);

        // Insert property which can be used to filter.
        $v = mt_rand(1, 3);
        $arr_stockprop = array(
          'stock_id' => $stock['stock_id'],
          'type_id' => $propvaltype,
          'value' => $awesome[$v],
        );
        chado_insert_record('stockprop', $arr_stockprop);
      }
    }
}
