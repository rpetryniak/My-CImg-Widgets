# ===================
# Short introduction:
# -------------------

  'MyCImgWidgets' is still a working name for a set of sample applications using several CImg "widgets" I prepared for my own needs. These "widgets" are very useful for me, and that is why I publish it for others.

  If you want to join and develop this "widgets" and example together, please contact me at email rpetryniak [at] gmail.com.

  More info: 

# =====================
# Author and greetings:
# ---------------------

  Rafał Petryniak (http://sites.google.com/site/rafalpetryniakresearch/)

  This source code could be created thanks to early efforts of CImg project leader (David Tschumperle) and other project team members. Read more: http://cimg.sourceforge.net/.

# =========
# Licenses:
# ---------

  The source code of 'MyCImgWidgets' (widgets and examples) can be used for any purpose, provided respect the CImg Library license (LGPL like license). The exception is header file 'include/queryhaar.h' because I rewrite most query image engine from Digikam. This file is GPL licenced.

# ========================
# Technical specification:
# ------------------------  

  MyImageCanvas     - inherits and expands CImg class. It displays the image, mask (binary image), the help frame, coordinates and some statistics.
  MyImageWindow     - inherits and expands CImgDisplay class. It supports an additional keyboard and mouse events.
  MyImageInfo       - simple structure which gathers information about the picture (for example, the path to the file).
  MyImageList       - vector of MyImageInfo objects. It hasposibility to load images from file list or database.
  MyImageListCanvas - inherits and expands CImg class. Displays thumbnails of images.
  MyImageListWindow - inherits and expands CImgDisplay class. Allows you to scroll through images, zoom thumbnails, select the image, and even run processing algorithm for all images.

# =====================
# Example applications:
# ---------------------

  I have prepared some simple applications to demonstrate how to use described above widgets.

  * image_viewer      - It demonstrates the possibility of usage 'MyImageCanvas' and 'MyImageWindow' widgets. Allow to display single image with mask (binary image). Mask can be modified with two different tools: circle tool and drawing tool. The program allows you to run image processing algorithm. By default, this is a thresholding operation. In addition, it is possible to display
  * image_list_viewer - It demonstrates the possibility of usage 'MyImageCanvas' and 'MyImageWindow' widgets. 
  * image_list_query  - Similar to 'image_list_query' with additional posibility to query images (sorting them according to similarity to the reference image). Query algorithm is based on the Haar transform and has been rewritten from DigiKam photo management program. 
  * image_db_viewer   - Similar to 'image_list_query' but images are stored in Sqlite database.
  * image_db_query    - Extends 'image_db_viewer' to query capabilities (like 'image_list_query').

# ===============================
# Preparing for building process:
# -------------------------------
  
  Important: This source code was tested only on Linux, but should work proper also on Windows and Mac.

  Basic functionality of 'MyCImgWidgets' require only CImg.h header file (tested for 1.4.2 version).
  Examples: 'image_db_query' and 'image_db_viewer' additionaly depends on sqlite3 development library.
  For building process CMake configuration was prepared.

  Note:
    Personally, I develop applications on different computers and always distinguish the source code from the temporary data (binary file, output file) and test input data. Very helpful for this purpose are properly set environment variables.
    Of course environment variables are obviously not required. Examples can be compiled using gcc traditionally. The relevant compilation commands can be found at the end of each application file. Also CMake build does not require setting these variables. 

  * Prepare the directory structure for sources, temporary files and test files:

    mkdir -p /home/$USER/work/projects/{project_src,project_tmp,project_build,project_data}

  * Set the following environment variables:
  
    export PROJECT_SRC=/home/$USER/work/projects/project_src
    export PROJECT_TMP=/home/$USER/work/projects/project_tmp
    export PROJECT_BUILD=/home/$USER/work/projects/project_build
    export PROJECT_DATA=/home/$USER/work/projects/project_data


# ==============
# Prerequisites:
# --------------

  * 'MyCImgWidgets' has been tested with CImg library in version 1.4.2.
  * Please make sure you have the Sqlite libraries installed. In Ubuntu Linux you can do this by a command:

    sudo apt-get install sqlite3 libsqlite3-0 libsqlite3-dev

# ==================
# First compilation:
# ------------------

  * Place the source in the appropriate directory:

    cp -r MyCImgWidgets/ $PROJECT_SRC

  * Prepare building directory:
  
    cd $PROJECT_BUILD
    mkdir MyCImgWidgets
    cd MyCImgWidgets

  * Run CMake

    cmake-gui $PROJECT_SRC/MyCImgWidgets
      build_type  Release (niekoniecznie)

    make

# ======================
# Next time compilation:
# ----------------------

  * Run only this command
  
    cd $PROJECT_BUILD/MyCImgWidgets && make

# ====================
# Sample input images:
# --------------------

  You can find description how to prepare test input data in directory: MyCImgWidgets/test_data/

# ================================
# The creation of data structures:
# --------------------------------

  * Prepare temporary directory:

    cd $PROJECT_TMP
    mkdir MyCImgWidgets
    cd MyCImgWidgets

  * Create database file and appropriate table structure:

    rm image_db_casimage_selected.sqlite3

    sqlite3 image_db_casimage_selected.sqlite3
    create table imagelist (id integer primary key autoincrement, image blob, mask blob, filename char, dimx integer, dimy integer, dimz integer, dimv integer);

    select * from imagelist;
    select id,filename,dimx,dimy,dimz,dimv from imagelist;

    .q

  * Go back to building directory:

    cd $PROJECT_BUILD/MyCImgWidgets
  