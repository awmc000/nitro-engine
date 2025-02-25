Changelog
=========

Version 0.8.2 (2023-04-20)
--------------------------

- Decouple mesh objects from model objects. This simplifies cloning models.
  Previously it was needed to preserve the original object as long as you wanted
  to use the clones. Now, it can be deleted and Nitro Engine won't free the mesh
  until all clones have been deleted.

- Support vertex color commands in ``obj2dl``. This can't be used at the same
  time as normals.

- Improve examples. A script has been added to convert all assets used by the
  examples. Also, the NFlib example has been updated to work with upstream
  NFlib.

- Support BlocksDS.

- A few minor fixes.

Version 0.8.1 (2022-11-10)
--------------------------

Models and materials:

- Improve support of specular properties of materials and add an example of how
  to use it for metalic objects.

- Fix material cloning:

  - Copy material properties apart from just the texture.

  - Assign palettes to materials instead of textures, so that a single texture
    can have multiple textures. You can load a texture to a material, clone the
    material, and assign a different palette to the cloned material.

- Support loading compressed textures and add an example of how to load them.
  Note that ``img2ds`` doesn't support this format yet. Until that support is
  added, compressed texture support should be considered experimental.

- Add example of how to use NFlib at the same time as Nitro Engine. NFlib is a
  library that has support for 2D graphics, which complements the 3D hardware
  support of Nitro Engine.

Other:

- Rename a few functions for consistency. The old names have been kept for
  compatibility, but they will be removed.

- Added some enumerations to help remember the names to be used as function
  arguments.

- The general-purpose allocator has been improved a lot to support compressed
  textures. This is needed due to the special way to load them to VRAM.
  Extensive tests for the allocator have also been added.

- Many internal changes to simplify the code and remove dependencies on libnds
  functions.

Version 0.8.0 (2022-10-21)
--------------------------

Models and materials:

- Add support for MD5 animated models (thanks to
  https://github.com/AntonioND/dsma-library): Introduce tool ``md5_to_dsma`` to
  convert them to a format that Nitro Engine can use.

- Add support for OBJ static models: Introduce tool obj2dl to convert them to a
  format that Nitro Engine can use.

- Introduce tool ``img2ds`` to convert images in many popular formats (PNG, JPG,
  BMP, etc) to DS textures (PNG is still recommended over other formats, as it
  supports alpha better than other formats).

- Drop support for MD2 models (static or animated).

- Remove NDS Model Exporter, Nitro Texture Converter, md2_to_bin and md2_to_nea.
  The animation system has been refactored (but NEA files don't work anymore, so
  you need to update your code anyway).

General:

- Huge cleanup of code style of the codebase.

- Cleanup of all examples. Add the original assets and textures used in all
  examples to the repository, along scripts to convert them to the formats used
  by Nitro Engine.

- Implement a better way to have debug and release builds of the library.

Notes:

- You can still use textures converted with Nitro Texture Converter or NDS Model
  Exporter, and you can still use any model exported with NDS Model Exporter or
  ``md2_to_bin``. However, support for NEA files has been removed (it had awful
  performance, and it was just a bad way to do things), so any file converted by
  ``md2_to_nea`` won't work anymore.

- The reason to replace most tools is that several people had issues building
  them. All the new tools are written in Python, so they don't need to be
  compiled.

Version 0.7.0 (2019-6-14)
-------------------------

- Pushed to GitHub.

- Major cleanup of code.

- Clarify license.

- Reworked tools to build under Linux and Windows.

Version 0.6.1 (2011-9-1)
------------------------

- Fixed identation in all code. Now it isn't a pain to read it (not as much as
  before, :P). Also, a few warnings fixed (related to libnds new versions).

Version 0.6.0 (2009-6-30)
-------------------------

- The functions used to modify textures and palettes now return a pointer to the
  data so that you can modify them easily.

- Each material can have different propierties (amient, diffuse...). You can set
  the default ones, the propierties each new material will have, and then you
  can set each material's propierties individually.

- New texture and palette allocation system, it is faster and better.
  Defragmenting functions don't work now, but I'll fix them for the next
  version.

- Added a debug system. You can compile Nitro Engine in "debug mode" and it will
  send error messages to the function you want. Once you have finished debugging
  or whatever, just recompile Nitro Engine without debug mode.

- Window system renamed to Sprite system. You can set a rotation and a scale for
  each one.

- The most important thing... The animation system has been improved, and now
  animated models are drawn using linear interpolation (you can disable it,
  anyway).

- As a result, I've modified the converters, so you'll have to convert yout
  animated models again.

Version 0.5.1 (2009-1-28)
-------------------------

- Minor bugfixes.

Version 0.5.0 (2009-1-5)
------------------------

- Text system and camera system optimized. New functions for the camera system.

- ``NE_TextPrintBox()`` and ``NE_TextPrintBoxFree()`` slightly changed. They can
  limit the text drawn to a number of characters set by the coder.

- Some functions made internal. Don't use them unless you know what you are
  doing.

- Fixed (?) at least the 2D projection.

- HBL effects fixed.

- Touch test functions.

- ``NE_UPDATE_INPUT`` removed.

- It now supports any BMP size, and BMP with 4 bits of depth.

- Arrays made pointers, so there is more memory free when you are not using
  Nitro Engine. You can also configure the number of objects of each systems you
  are going to use.

- ``NE_TextPalette`` replaced by ``NE_Palette``.

- You can clone materials to use the same texture with different colors. This
  doesn't have the problems of cloning models.

- Added functions to remove all palettes and textures.

- Fixed ``NE_End()``.

- NE can free all memory used by it, and the coder can tell NE how much memory
  to use.

- Texture drawing system improved a bit.

- ``NE_PolyFormat()`` simplified.

- Some bugfixes, code reorganized, define lists converted into enums.

- Clear bitmap supported, this is used to display an bitmap as rear plane. Each
  pixel can have different depth. This needs 2 VRAM banks to work.

- Solved some problems with 2D system and culling.

- Nomad ``NDS_Texture_Converter`` is no longer included, if you want it, look for it
  in Google.

- Added Nitro Texture Converter, made by me. Open source, and it exports various
  levels of alpha in the textures that can handle it. It does only accept PNG
  files.

- NE now accepts any texture size. ``NE_SIZE_XXX`` defines removed as they are
  not needed now.

- Added a couple of examples.

Version 0.4.2 (2008-12-14)
--------------------------

- Fixed 2D system (textures were displaying wrong on 2D quads) and text system
  (paletted textures sometimes were drawn without palette).

- Modified ``MD2_2_NEA``, ``MD2_2_BIN`` and ``bin2nea`` to work in linux. Thanks
  to nintendork32.

- Added a couple of examples.

Version 0.4.1 (2008-12-12)
--------------------------

- Lots of bugfixes. Specially, UV coordinates swapping fixed.

- Added a function to draw on RGBA textures ^_^.

- Fixed ``MD2_2_NEA`` and ``MD2_2_BIN``. You'll have to convert again your
  models.

- Updated to work with latest libnds. There is a define in case you want to use
  an older version.

Version 0.4.0 (2008-10-15)
--------------------------

- Added ``MD2_2_NEA`` (converts an MD2 model into a NEA file that can used by
  Nitro Engine) and ``MD2_2_BIN`` (Converts the first frame of an MD2 model
  into a display list). Display lists created by them are really optimized.

- Updated ``DisplayList_Fixer``. Now it can remove normal commands too.

- Added a text system. It can use fonts of any size. ^^

- Added some simple API functions (buttons, check boxes, radio buttons and slide
  bars).

- Fixed 2D projection.

- Removed some internal unused functions to save space, and made 'inline' some
  of the rest.

- Functions that used float parameters modified so they use integers now. You
  can still use some wrappers if you want to use floats. This will let the
  compiler try to optimize the code.

- Animated and static models are now the same. You can move, rotate, etc them
  with the same functions.

- Now, you can 'clone' models so you can save a lot of RAM if they are repeated.

- Renamed lots of model functions. Take a look at new examples or documentation.

- ``NE_Color`` struct removed (I don't even know why I created it...).

- Examples updated to work with last version and added examples of clonning
  models, API and text system.

- libnds' console is not inited with Nitro Engine. You will have to init it
  yourself with ``NE_InitConsole()`` or libnds' functions.

Version 0.3.0 (2008-9-16)
-------------------------

- Support for animated models (NEA format) and a program to make a new NEA file
  from many models (in bin format).

- 2D over 3D system. You can draw easily quads (with or without texture) as if
  they were drawn using 2D.

- Basic physics engine (gravity, friction and collitions). It does only support
  bounding boxes for now.

- Added a function to delete all models, animated or not.

- Window system, very simple. I will make some API functions in next versions.

- Nitro Engine compiled as a library to include it easier in projects and save
  space.

- Examples folder organized a bit and added some new examples.

- Nitro Engine is now licensed under the BSD license.

Version 0.2.0 (2008-8-31)
-------------------------

- Added effects like fog and shading, functions to load BMP files and convert
  them in textures and more examples.

Version 0.1 (2008-8-24)
-----------------------

- Includes 2 examples, documentation, tools to export models from the PC, the
  license and full source.
