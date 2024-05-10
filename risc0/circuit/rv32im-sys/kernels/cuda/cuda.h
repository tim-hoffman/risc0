// Copyright 2024 RISC Zero, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstring>
#include <cuda_runtime.h>
#include <stdexcept>

template <typename... Types> inline std::string fmt(const char* fmt, Types... args) {
  size_t len = std::snprintf(nullptr, 0, fmt, args...);
  std::string ret(++len, '\0');
  std::snprintf(&ret.front(), len, fmt, args...);
  ret.resize(--len);
  return ret;
}

#define CUDA_OK(expr)                                                                              \
  do {                                                                                             \
    cudaError_t code = expr;                                                                       \
    if (code != cudaSuccess) {                                                                     \
      auto file = std::strstr(__FILE__, "sppark");                                                 \
      auto msg = fmt("%s@%s:%d failed: \"%s\"",                                                    \
                     #expr,                                                                        \
                     file ? file : __FILE__,                                                       \
                     __LINE__,                                                                     \
                     cudaGetErrorString(code));                                                    \
      throw std::runtime_error{msg};                                                               \
    }                                                                                              \
  } while (0)

class CudaStream {
private:
  cudaStream_t stream;

public:
  CudaStream() { cudaStreamCreate(&stream); }
  ~CudaStream() { cudaStreamDestroy(stream); }

  inline operator cudaStream_t() const { return stream; }
};

struct LaunchConfig {
  dim3 grid;
  dim3 block;
  size_t shared;

  LaunchConfig(dim3 grid, dim3 block, size_t shared = 0)
      : grid(grid), block(block), shared(shared) {}
  LaunchConfig(int grid, int block, size_t shared = 0) : grid(grid), block(block), shared(shared) {}
};

LaunchConfig getSimpleConfig(uint32_t count);
