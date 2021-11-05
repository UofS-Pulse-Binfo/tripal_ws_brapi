# Tripal WS BrAPI

A Drupal module that implements the Breeding API (BrAPI) standardized specifications for Tripal websites that host genotypic/phenotypic data stored in a CHADO Database. BrAPI is an interface for data interchange between crop breeding applications.		

 - Breeding API (BrAPI)	http://docs.brapi.apiary.io
 - Tripal: http://tripal.info
 - CHADO: http://gmod.org/wiki/Chado
 - Drupal: https://www.drupal.org

## Requirements

1. Tripal 3 (with Tripal Web Service extension)	http://tripal.info
2. CHADO	http://gmod.org/wiki/Chado
3. Drupal 7.x	https://www.drupal.org

## Library Requirements - To use BrAPI Applications (BRAPPS)
1. Bootstrap v3.3.7 https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/3.3.7/js/bootstrap.min.js
2. D3 v4.12.0 https://cdnjs.cloudflare.com/ajax/libs/d3/4.12.0/d3.js
3. DataTables v1.10.16 https://cdn.datatables.net/1.10.16/js/jquery.dataTables.min.js
4. jQuery JavaScript Library v3.2.1 https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.js
5. BRAPPS: https://github.com/UofS-Pulse-Binfo/BrAPI-Graphical-Filtering
6. BRAPPS: Solgenomics/BrAPI-Study-Comparison https://github.com/solgenomics/BrAPI-Study-Comparison

NOTE: Please use the Graphical Filter BRAPPS version found in UofS-Pulse-Binfo repository while
our pull request is being reviewed by Solgenomics Team. We will update this link to direct to
https://github.com/solgenomics/BrAPI-Graphical-Filtering as soon as our request has been merged.

## Installation

This module is installed by cloning it [your drupal site]/sites/all/modules and enabling it through the Drupal Administrative UI.

## Installation - To use BrAPI Applications

Dowload all libraries listed in Library Requirements - To use BrAPI Applications (BRAPPS) and save each item into your Drupal Libraries directory following folder names below.

1. Bootstrap           - sites/all/libraries/bootstrap_3_3/
2. D3                  - sites/all/libraries/d3_4/
3. DataTables          - sites/all/libraries/datatables_1/
4. jQUery              - sites/all/libraries/jquery_3_2/
5. Graphical Filtering - sites/all/libraries/BrAPI-Graphical-Filtering/
5. Study Comparison    - sites/all/libraries/BrAPI-Study-Comparison/

## Usage

You can access the web-serivces provided by this module by going to `https://[your drupal site]/web-services`. The list of web-service calls available can be retrieved by going to the `https://[your drupal site]/web-services/brapi/v[majorversion]/calls`.

## Documentation

There is an extensive user guide available here: https://tripal-ws-brapi.readthedocs.io/en/latest/

## Automated Testing ![Run PHPUnit Tests](https://github.com/UofS-Pulse-Binfo/tripal_ws_brapi/workflows/Run%20PHPUnit%20Tests/badge.svg)

There is automated testing included using PHPUnit Tests with Guzzle. Additionally, there is a helper module providing test data for [manual demonstration](https://tripal-ws-brapi.readthedocs.io/en/latest/topics/contribute.html#using-the-testing-helper-module) of the module.

## Funding

This work is supported by Saskatchewan Pulse Growers [grant: BRE1516, BRE0601], Western Grains Research Foundation, Genome Canada [grant: 8302, 16302], Government of Saskatchewan [grant: 20150331], and the University of Saskatchewan.

## Citation

Tan R., Sanderson, L.A. (2020). Tripal WS BrAPI: Integration of Tripal with BrAPI through core Tripal web-services. DEVELOPMENT VERSION. University of Saskatchewan, Pulse Crop Research Group, Saskatoon, SK, Canada.
