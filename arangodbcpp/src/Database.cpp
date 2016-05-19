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

#include <sstream>

#include "arangodbcpp/Database.h"

namespace arangodb {

namespace dbinterface {

const std::string Database::httpDbApi{"/_api/database"};

Database::Database(const Server::SPtr& srv, const std::string& name)
    : _server{srv}, _name(name) {}

Database::Database(const Server::SPtr& srv, std::string&& name)
    : _server{srv}, _name(name) {}

//
//      Get the core database url
//
ConnectionBase::Url Database::databaseUrl(const std::string& tail) const {
#ifdef FUERTE_CONNECTIONURL
  ConnectionUrl res{_server->hostUrl()};
  if (!_name.empty()) {
    res.setDbName(_name);
  }
#else
  std::string res = _server->hostUrl();
  if (!_name.empty()) {
    res += "/_db/" + _name;
  }
#endif
  return res += tail;
}

//
// Configure to create a Database using the VPack configuration data
//
void Database::create(const ConnectionBase::SPtr& p,
                      const ConnectionBase::VPack& data, const bool bAsync) {
  ConnectionBase::Url val{_server->hostUrl() + httpDbApi};
  ConnectionBase& conn = p->reset();
  conn.setUrl(val);
  conn.setPostReq();
  conn.setPostField(data);
  conn.setBuffer();
  conn.setAsynchronous(bAsync);
}

//
// Configure to create a Database using the configured name
//
void Database::create(const ConnectionBase::SPtr& p, const bool bAsync) {
  ConnectionBase& conn = p->reset();
  {
    ConnectionBase::Url val{_server->hostUrl() + httpDbApi};
    conn.setUrl(val);
  }
  {
    std::string val = "{ \"name\" : \"" + _name + "\" }";
    conn.setPostField(val);
  }
  conn.setPostReq();
  conn.setBuffer();
  conn.setAsynchronous(bAsync);
}

//
// Configure to get info on the current Database
//
void Database::info(const ConnectionBase::SPtr& p, const bool bAsync) {
  ConnectionBase::Url url{_server->hostUrl() + httpDbApi + "/current"};
  ConnectionBase& conn = p->reset();
  conn.setUrl(url);
  conn.setGetReq();
  conn.setBuffer();
  conn.setAsynchronous(bAsync);
}

//
// Configure to drop a Database using the configured name
//
void Database::remove(const ConnectionBase::SPtr& p, const bool bAsync) {
  ConnectionBase::Url url{_server->hostUrl() + (httpDbApi + '/' + _name)};
  ConnectionBase& conn = p->reset();
  conn.setUrl(url);
  conn.setDeleteReq();
  conn.setBuffer();
  conn.setAsynchronous(bAsync);
}
}
}
