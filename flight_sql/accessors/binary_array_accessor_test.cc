// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "arrow/testing/gtest_util.h"
#include "binary_array_accessor.h"
#include "gtest/gtest.h"

namespace driver {
namespace flight_sql {

using namespace arrow;
using namespace odbcabstraction;

TEST(BinaryArrayAccessor, Test_CDataType_BINARY_Basic) {
  std::vector<std::string> values = {"foo", "barx", "baz123"};
  std::shared_ptr<Array> array;
  ArrayFromVector<BinaryType, std::string>(values, &array);

  BinaryArrayFlightSqlAccessor<CDataType_BINARY> accessor(array.get());

  size_t max_strlen = 64;
  char buffer[values.size() * max_strlen];
  ssize_t strlen_buffer[values.size()];

  ColumnBinding binding(CDataType_BINARY, 0, 0, buffer, max_strlen,
                        strlen_buffer);

  ASSERT_EQ(values.size(),
            accessor.GetColumnarData(&binding, 0, values.size(), 0));

  for (int i = 0; i < values.size(); ++i) {
    ASSERT_EQ(values[i].length(), strlen_buffer[i]);
    // Beware that CDataType_BINARY values are not null terminated.
    // It's safe to create a std::string from this data because we know it's ASCII,
    // this doesn't work with arbitrary binary data.
    ASSERT_EQ(values[i],
              std::string(buffer + i * max_strlen,
                          buffer + i * max_strlen + strlen_buffer[i]));
  }
}

TEST(BinaryArrayAccessor, Test_CDataType_BINARY_Truncation) {
  std::vector<std::string> values = {
      "ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF"};
  std::shared_ptr<Array> array;
  ArrayFromVector<BinaryType, std::string>(values, &array);

  BinaryArrayFlightSqlAccessor<CDataType_BINARY> accessor(array.get());

  size_t max_strlen = 8;
  char buffer[values.size() * max_strlen];
  ssize_t strlen_buffer[values.size()];

  ColumnBinding binding(CDataType_BINARY, 0, 0, buffer, max_strlen,
                        strlen_buffer);

  std::stringstream ss;
  int64_t value_offset = 0;

  // Construct the whole string by concatenating smaller chunks from
  // GetColumnarData
  do {
    ASSERT_EQ(1, accessor.GetColumnarData(&binding, 0, 1, value_offset));
    ASSERT_EQ(values[0].length(), strlen_buffer[0]);

    int64_t chunk_length = std::min(static_cast<int64_t>(max_strlen),
                                    strlen_buffer[0] - value_offset);

    // Beware that CDataType_BINARY values are not null terminated.
    // It's safe to create a std::string from this data because we know it's ASCII,
    // this doesn't work with arbitrary binary data.
    ss << std::string(buffer, buffer + chunk_length);
    value_offset += chunk_length;
  } while (value_offset < strlen_buffer[0]);

  ASSERT_EQ(values[0], ss.str());
}

} // namespace flight_sql
} // namespace driver