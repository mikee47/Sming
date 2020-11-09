Getting Started: ESP32
======================

Requirements
------------

In order to be able to compile for the ESP32 architecture you should have ESP-IDF v4.1 installed.

Get the latest `release/v4.1` branch. This can be done using the following command::

    git clone -b release/v4.1 --recursive https://github.com/espressif/esp-idf.git

Once the command above finishes follow the installation steps described in the
`ESP-IDF documentation <https://docs.espressif.com/projects/esp-idf/en/v4.0/get-started/index.html#installation-step-by-step>`__.

Building
--------

Make sure that the IDF_PATH environmental variable is set. On Linux you can use for example the following command::

    export IDF_PATH=path/to/esp-idf

Replace `path/to` with the correct path.

.. note::

   You do not need to run IDF export.sh - tool paths are handled by the Sming build system
   which simplifies use within the Eclipse IDE.

   If you need to customise the tool paths, 
   XXXXXXXXXXXXXXXXXX

Build the framework and application as usual, specifying :envvar:`SMING_ARCH` =Esp32. For example::

   cd $SMING_HOME/../samples/Basic_Serial
   make SMING_ARCH=Esp32

This builds the application. Once built the application needs to be flashed on a real Esp32 microcontroller to run.
Flashing can be done using the following command::

  make flash


.. note::

   If you have an ESP32-S2 device you'll need to change :envvar:`ESP_VARIANT`::
   
      make ESP_VARIANT=esp32s2


See :component-esp32:`esp32` for further details.
