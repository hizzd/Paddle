/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/memory/detail/system_allocator.h"

#include <memory>
#include <vector>

#include "gflags/gflags.h"
#include "gtest/gtest.h"

DECLARE_bool(use_pinned_memory);

void TestAllocator(paddle::memory::detail::SystemAllocator& a, size_t size) {
  bool freed = false;
  {
    void* p = a.Alloc(size);
    if (size > 0) {
      EXPECT_NE(p, nullptr);
    } else {
      EXPECT_EQ(p, nullptr);
    }

    int* i = static_cast<int*>(p);
    std::shared_ptr<int> ptr(i, [&](void* p) {
      freed = true;
      a.Free(p, size);
    });
  }
  EXPECT_TRUE(freed);
}

TEST(CPUAllocator, NoLockMem) {
  FLAGS_use_pinned_memory = false;
  paddle::memory::detail::CPUAllocator a;
  TestAllocator(a, 2048);
  TestAllocator(a, 0);
}

TEST(CPUAllocator, LockMem) {
  FLAGS_use_pinned_memory = true;
  paddle::memory::detail::CPUAllocator a;
  TestAllocator(a, 2048);
  TestAllocator(a, 0);
}

#ifndef PADDLE_ONLY_CPU
TEST(GPUAllocator, NoStaging) {
  FLAGS_use_pinned_memory = false;
  paddle::memory::detail::GPUAllocator a;
  TestAllocator(a, 2048);
  TestAllocator(a, 0);
}
TEST(GPUAllocator, Staging) {
  FLAGS_use_pinned_memory = true;
  paddle::memory::detail::GPUAllocator a;
  TestAllocator(a, 2048);
  TestAllocator(a, 0);
}
#endif  // PADDLE_ONLY_CPU
