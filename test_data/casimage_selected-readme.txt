This file describes how to prepare a test file with path to input data.
You can of course make your own file with direct paths to your images.

1. Download example data 'Casimage Database (selected images)' from:

  http://sites.google.com/site/rafalpetryniakresearch/software/sample-data

2. After unpacking, place them in a following directory:

  $PROJECT_DATA/test_data/casimage_selected

3. Prepare a list of full paths to all files by running following script:

  cd $PROJECT_DATA/test_data/casimage_selected/
  ls -d $PWD/* > casimage-selected.txt
  mv casimage-selected.txt $PROJECT_SRC/MyCImgWidgets/test_data

4. You can use 'test_data/casimage-selected.txt' file for testing provided application in 'app/' directory.
