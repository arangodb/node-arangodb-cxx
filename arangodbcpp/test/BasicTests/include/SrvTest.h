////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author John Bufton
////////////////////////////////////////////////////////////////////////////////

#ifndef SRVTEST_H
#define SRVTEST_H

#include <gtest/gtest.h>

#include "arangodbcpp/Server.h"

class SrvTest : public testing::Test {
 public:
  typedef arangodb::dbinterface::Server Server;
  typedef arangodb::dbinterface::ConnectionBase ConnectionBase;
  SrvTest();
  virtual ~SrvTest();

 protected:
  const ConnectionBase::VPack getDbVersion();

 private:
  Server::SPtr _pSrv;
  ConnectionBase::SPtr _pCon;
};

inline SrvTest::~SrvTest() {}

#endif  // SRVTEST_H
