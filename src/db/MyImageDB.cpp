#include "MyImageDB.h"

//public:

MyImageDB::MyImageDB() {}

MyImageDB::MyImageDB(const char* db_name)
{
  open(db_name);
}

MyImageDB::~MyImageDB() {
  close();
}

void MyImageDB::open(const char* db_name)
{
  printf("Opening database ................... \n");
  char *zErrMsg = 0;
  int rc;
  rc = sqlite3_open(db_name, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
}

void MyImageDB::close()
{
  printf("Closing database ................... \n");
  sqlite3_close(db);
}

void MyImageDB::update(MyImageList<unsigned char> imagelist)
{
  startTransaction();

  int status = 0, counter=0;
  for (int i=0; i < imagelist.size(); i++) {
    status = update( imagelist.at(i) );
    if (status>0) counter++;
    printf("\rUpdating database .................. [ %4d ]",counter);
  }
  endTransaction();
  printf("\n");
}

int MyImageDB::update(MyImageInfo<unsigned char> imageinfo)
{
  if (imageinfo.maskChanged) {
    sqlite3_stmt *update_stmt;
    char update_str[] = "update imagelist set mask=? where id=?";

    sqlite3_prepare(db, update_str, -1, &update_stmt, NULL);

    sqlite3_bind_blob(update_stmt, 1, imageinfo.mask.data(),  imageinfo.mask.size()     * sizeof(unsigned char), SQLITE_STATIC);
    sqlite3_bind_int (update_stmt, 2, imageinfo.db_id);

    sqlite3_step (update_stmt);
    sqlite3_reset(update_stmt);
    return 1;
  } else
    return 0;
}

void MyImageDB::addImage (MyImageInfo<unsigned char> imageinfo)
{
  sqlite3_stmt *insert_stmt;
  char insert_str[] = "insert into imagelist(image,mask,filename,dimx,dimy,dimz,dimv) values (?,?,?,?,?,?,?)";

  sqlite3_prepare(db, insert_str, -1, &insert_stmt, NULL);

  const char* filename = imageinfo.fileName.c_str();

  //printf(filename);

  //Inserting images to database
  sqlite3_bind_blob(insert_stmt, 1, imageinfo.image.data(), imageinfo.image.size()  * sizeof(unsigned char), SQLITE_STATIC);
  sqlite3_bind_blob(insert_stmt, 2, imageinfo.mask.data(),  imageinfo.mask.size()   * sizeof(unsigned char), SQLITE_STATIC);
  sqlite3_bind_text(insert_stmt, 3, filename,              strlen(filename)         * sizeof(const char),    SQLITE_STATIC);
  sqlite3_bind_int (insert_stmt, 4, imageinfo.image.width());
  sqlite3_bind_int (insert_stmt, 5, imageinfo.image.height());
  sqlite3_bind_int (insert_stmt, 6, imageinfo.image.depth());
  sqlite3_bind_int (insert_stmt, 7, imageinfo.image.spectrum());
  sqlite3_step (insert_stmt);
  sqlite3_reset(insert_stmt);
}


void MyImageDB::addImages(MyImageList<unsigned char> imagelist)
{
  startTransaction();

  for (int i=0; i < imagelist.size(); i++) {
    addImage( imagelist.at(i) );
    printf("\rAdding images to database .......... [ %4d ]",i);
  }

  endTransaction();
  printf("\n");
}

MyImageList<unsigned char> MyImageDB::getImages()
{
  int dx,dy,dz,dv;
  int rownum=0, num_bytes;
  const unsigned char *filename;
  const unsigned char *data_buffer;

  sqlite3_stmt *select_stmt;
  char select_str[] = "select id, image,mask,filename,dimx,dimy,dimz,dimv from imagelist";

  sqlite3_prepare(db, select_str,-1, &select_stmt, NULL);

  MyImageList<unsigned char> imagelist;

  int index=0, id=0;
  while (sqlite3_step(select_stmt) == SQLITE_ROW) {
    //image id
    id = sqlite3_column_int(select_stmt, 0);
    //reading file name
    filename = sqlite3_column_text(select_stmt, 3);
    //reading image size
    dx = sqlite3_column_int(select_stmt, 4);
    dy = sqlite3_column_int(select_stmt, 5);
    dz = sqlite3_column_int(select_stmt, 6);
    dv = sqlite3_column_int(select_stmt, 7);
    //fetching image data
    int num_bytes1 = sqlite3_column_bytes(select_stmt,1);
    num_bytes = num_bytes1 / sizeof(unsigned char);
    data_buffer = new unsigned char[num_bytes1];
    data_buffer = (const unsigned char*)sqlite3_column_blob(select_stmt, 1);
    CImg<unsigned char> tmp1(data_buffer,dx,dy,dz,dv);

    //fetching mask image data
    int num_bytes2 = sqlite3_column_bytes(select_stmt,2);
    num_bytes = num_bytes2 / sizeof(unsigned char);
    data_buffer = new unsigned char[num_bytes2];
    data_buffer = (const unsigned char*)sqlite3_column_blob(select_stmt, 2);
    CImg<unsigned char> tmp2(data_buffer,dx,dy,dz,dv);

    imagelist.push_back( MyImageInfo<unsigned char>(tmp1,tmp2, reinterpret_cast<const char*>(filename),id ) );
    imagelist.orderList.push_back(index++);
    imagelist.group.push_back(0);
    imagelist.score.push_back(0);

    printf("\rLoading images from database ....... [ %4d ]",index-1);
  }

  printf("\n");
  return imagelist;
}

//private:

void MyImageDB::startTransaction()
{
  char begin_str[]  = "begin transaction";
  sqlite3_exec(db, begin_str, NULL, NULL, NULL);
}

void MyImageDB::endTransaction()
{
  char commit_str[] = "commit transaction";
  sqlite3_exec(db, commit_str, NULL, NULL, NULL);
}

void MyImageDB::deleteAll()
{
  startTransaction();

  char deleteall_str[] = "delete from imagelist";
  sqlite3_exec(db, deleteall_str, NULL, NULL, NULL);

  endTransaction();
}
