===========
win32_tools
===========

About
=====

A repository of personal command line programs for Windows.

- ``etime`` - Measure execution time of a given command. Inspired by \*nix
  ``time``.

- ``watchcmd`` - Listen for file changes in the current directory and if
  detected, run the specified command.

Build Instructions
==================

To build using MSVC with optimizations enabled, use::

   build_win32_msvc.bat optimized

To build using Clang with optimizations enabled, use::

   build_win32_clang.bat optimized

License
=======

The Unlicense. See ``UNLICENSE`` file for more details.
