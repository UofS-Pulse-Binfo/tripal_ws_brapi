Contributing
==============

We’re excited to work with you! Post in the issues queue with any questions, feature requests, or proposals.

Automated Testing
--------------------

This module uses `Tripal Test Suite <https://tripaltestsuite.readthedocs.io/en/latest/installation.html#joining-an-existing-project>`_. To run tests locally:

.. code:: bash

  cd MODULE_ROOT
  composer up
  ./vendor/bin/phpunit

This will run all tests associated with the Tripal WS BrAPI extension module. If you are running into issues, this is a good way to rule out a system incompatibility.

.. warning::

  It is highly suggested you ONLY RUN TESTS ON DEVELOPMENT SITES. We have done our best to ensure that our tests clean up after themselves; however, we do not guarantee there will be no changes to your database.

.. _demo-instructions:

Manual Testing (Demonstration)
--------------------------------

We have provided a `Tripal Test Suite Database Seeder <https://tripaltestsuite.readthedocs.io/en/latest/db-seeders.html>` to make development and demonstration of functionality easier. To populate your development database with fake germplasm data:

1. Install this module according to the instructions in this guide.
2. Run the database seeder to populate the database using the following commands:

  .. code::

    cd MODULE_ROOT
    composer up
    ./vendor/bin/tripaltest db:seed BrAPIDatabaseSeeder

4. To access the web services go to ``https:[your drupal site]/web-services`` assuming the default configuration.

.. warning::

  NEVER run database seeders on production sites. They will insert fictitious data into Chado.
