// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/arena.h"

namespace leveldb {

static const int kBlockSize = 4096;       //4kB

Arena::Arena()
    : alloc_ptr_(nullptr), alloc_bytes_remaining_(0), memory_usage_(0) {}

Arena::~Arena() {
  for (size_t i = 0; i < blocks_.size(); i++) {
    delete[] blocks_[i];
  }
}

char* Arena::AllocateFallback(size_t bytes) {
  /*
  判断需要申请的内存空间是否大于1/4块的的大小
  如果大于1/4的块大小，单独分配一块需要的内存大小的块，避免浪费太多空间
  否则，重新分配一块新的内存

  此函数仅在allocateAlign中使用，既当bytes大于剩余的空间时才会调用此函数
  */
  if (bytes > kBlockSize / 4) {
    // Object is more than a quarter of our block size.  Allocate it separately
    // to avoid wasting too much space in leftover bytes.
    //如果大于1/4的块大小，单独分配，避免浪费太多空间
    //内存中可能有很多块存储，也可能有单独分配的空间
    char* result = AllocateNewBlock(bytes);
    return result;
  }

  // We waste the remaining space in the current block.
  //浪费当前块的剩余空间，为了对齐，重新分配一块新的内存
  alloc_ptr_ = AllocateNewBlock(kBlockSize);
  alloc_bytes_remaining_ = kBlockSize;

  char* result = alloc_ptr_;
  alloc_ptr_ += bytes;//指向当前内存使用的空间
  alloc_bytes_remaining_ -= bytes;
  return result;
}

char* Arena::AllocateAligned(size_t bytes) {
  const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
  static_assert((align & (align - 1)) == 0,     
                "Pointer size should be a power of 2");
  //指针的大小应该是2的幂，因为指针的大小是8字节，所以align是8

  size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);//相当于取模
  //当前的内存地址和align-1做与操作，得到当前的内存地址和align的余数，
  //如果余数为0，说明当前的内存地址是align的倍数，不需要对齐，否则需要对齐，
  //需要对齐的字节数是align-current_mod，因为current_mod是小于align的，所以align-current_mod是大于0的
  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
  /*
    对齐内存地址，先得到当前的内存地址和align的余数，再得到和align相差的数，既slop
    更新当前的内存指针ptr时，就需要加上slop和需要分配的内存空间大小bytes
    来达到内存对齐的效果
  */
  size_t needed = bytes + slop;
  char* result;
  if (needed <= alloc_bytes_remaining_) {
    result = alloc_ptr_ + slop;
    alloc_ptr_ += needed;
    alloc_bytes_remaining_ -= needed;
  } else {
    // AllocateFallback always returned aligned memory
    result = AllocateFallback(bytes);
  }
  /*
  例如：
  align=8，ptr = 18，ptr % align = 2，slop = 6
  如果剩余的空间足够放下bytes和要对齐的数slop，则更新ptr和remaining
  否则
  */
  assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
  return result;
}

char* Arena::AllocateNewBlock(size_t block_bytes) {//得到16进制的内存空间
  char* result = new char[block_bytes];
  blocks_.push_back(result);
  memory_usage_.fetch_add(block_bytes + sizeof(char*),
                          std::memory_order_relaxed);//原子操作，每次新分配内存时都更新一次
  return result;
}

}  // namespace leveldb
