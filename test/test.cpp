/*
运行命令：g++ -std=c++11 test.cpp -lleveldb -o test
调试命令：g++ -std=c++11 -g test.cpp -lleveldb -o test

然后执行可执行文件：./test
若是调试：gdb ./test
  -gdb中加断点：break main         //在main函数处加断点
               break test.cpp:15  //在test.cpp文件的15行加断点

  -run                            //运行程序
  -next/n                         //单步执行
  -step/s                         //单步执行，进入函数内部
  -print  变量名                   //打印变量的值
  -continue                       //继续执行程序
  -finish                         //跳出当前函数
  -quit                           //退出gdb


*/

#include <cassert>
#include <iostream>
#include <iterator>

#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/slice.h"

int main() {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/test", &db);
  assert(status.ok());

  leveldb::WriteOptions write_options;
  leveldb::ReadOptions read_options;
  leveldb::Slice key{"1"};
  leveldb::Slice value{"666"};
  db->Put(write_options, key, value);
  std::string result;
  db->Get(read_options, key, &result);

  std::cout << "key: " << key.data() << ", value: " << result << std::endl;

  // iterator
  leveldb::Iterator* it = db->NewIterator(read_options);
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    std::cout << it->key().ToString() << ": " << it->value().ToString()
              << std::endl;
  }
  assert(it->status().ok());  // Check for any errors found during the scan
  delete it;

  return 0;
}