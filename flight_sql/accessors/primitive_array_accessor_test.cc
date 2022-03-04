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
#include "primitive_array_accessor.h"
#include "gtest/gtest.h"

namespace driver {
namespace flight_sql {

using namespace arrow;
using namespace odbcabstraction;

template <typename ARROW_ARRAY, CDataType TARGET_TYPE>
void TestPrimitiveArraySqlAccessor() {
  typedef typename ARROW_ARRAY::TypeClass::c_type c_type;

  std::vector<c_type> values = {0, 1, 2, 3, 127};

  std::shared_ptr<Array> array;
  ArrayFromVector<typename ARROW_ARRAY::TypeClass>(values, &array);

  PrimitiveArrayFlightSqlAccessor<ARROW_ARRAY, TARGET_TYPE> accessor(
      array.get());

  c_type buffer[values.size()];
  ssize_t strlen_buffer[values.size()];

  ColumnBinding binding(TARGET_TYPE, 0, 0, buffer, values.size(),
                        strlen_buffer);

  ASSERT_EQ(values.size(),
            accessor.GetColumnarData(&binding, 0, values.size(), 0));

  for (int i = 0; i < values.size(); ++i) {
    ASSERT_EQ(sizeof(c_type), strlen_buffer[i]);
    ASSERT_EQ(values[i], buffer[i]);
  }
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_Int64Array_CDataType_SBIGINT) {
  TestPrimitiveArraySqlAccessor<Int64Array, CDataType_SBIGINT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_Int32Array_CDataType_SLONG) {
  TestPrimitiveArraySqlAccessor<Int32Array, CDataType_SLONG>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_Int16Array_CDataType_SSHORT) {
  TestPrimitiveArraySqlAccessor<Int16Array, CDataType_SSHORT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_Int8Array_CDataType_STINYINT) {
  TestPrimitiveArraySqlAccessor<Int8Array, CDataType_STINYINT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_UInt64Array_CDataType_UBIGINT) {
  TestPrimitiveArraySqlAccessor<UInt64Array, CDataType_UBIGINT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_UInt32Array_CDataType_ULONG) {
  TestPrimitiveArraySqlAccessor<UInt32Array, CDataType_ULONG>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_UInt16Array_CDataType_USHORT) {
  TestPrimitiveArraySqlAccessor<UInt16Array, CDataType_USHORT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_UInt8Array_CDataType_UTINYINT) {
  TestPrimitiveArraySqlAccessor<UInt8Array, CDataType_UTINYINT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_FloatArray_CDataType_FLOAT) {
  TestPrimitiveArraySqlAccessor<FloatArray, CDataType_FLOAT>();
}

TEST(PrimitiveArrayFlightSqlAccessor, Test_DoubleArray_CDataType_DOUBLE) {
  TestPrimitiveArraySqlAccessor<DoubleArray, CDataType_DOUBLE>();
}

} // namespace flight_sql
} // namespace driver