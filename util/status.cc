// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/status.h"

#include <cstdio>

#include "port/port.h"

namespace leveldb {

const char* Status::CopyState(const char* state) {
  uint32_t size;
  std::memcpy(&size, state, sizeof(size));//将state中内存取四个字节给size
  //state中的前四个字节存储消息的长度，所以取前四个字节，此时size中存储的是消息长度
  char* result = new char[size + 5];//消息的长度+4位表示+1位错误码
  std::memcpy(result, state, size + 5);//然后将整体复制给result
  return result;
}

Status::Status(Code code, const Slice& msg, const Slice& msg2) {
  /*
    msg提供基本错误信息，msg2提供详细的错误信息
  */
  assert(code != kOk);
  const uint32_t len1 = static_cast<uint32_t>(msg.size());
  const uint32_t len2 = static_cast<uint32_t>(msg2.size());
  const uint32_t size = len1 + (len2 ? (2 + len2) : 0);
  //当len2不为空，说明有详细的错误
  //此时返回len2+2，作为日志信息，要加上冒号和空格，多占两位
  char* result = new char[size + 5];//前四位表示消息长度+1位错误码
  std::memcpy(result, &size, sizeof(size));//此时的size表示错误信息的长度
  //将错误信息长度以16进制给result
  result[4] = static_cast<char>(code);//错误码
  std::memcpy(result + 5, msg.data(), len1);
  if (len2) {
    result[5 + len1] = ':';
    result[6 + len1] = ' ';
    std::memcpy(result + 7 + len1, msg2.data(), len2);
  }
  state_ = result;
}

std::string Status::ToString() const {
  if (state_ == nullptr) {
    return "OK";
  } else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case kOk:
        type = "OK";
        break;
      case kNotFound:
        type = "NotFound: ";
        break;
      case kCorruption:
        type = "Corruption: ";
        break;
      case kNotSupported:
        type = "Not implemented: ";
        break;
      case kInvalidArgument:
        type = "Invalid argument: ";
        break;
      case kIOError:
        type = "IO error: ";
        break;
      default:
        std::snprintf(tmp, sizeof(tmp),
                      "Unknown code(%d): ", static_cast<int>(code()));
        type = tmp;
        break;
    }
    std::string result(type);
    uint32_t length;
    std::memcpy(&length, state_, sizeof(length));
    result.append(state_ + 5, length);
    return result;
  }
}

}  // namespace leveldb
