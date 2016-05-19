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

#include "arangodbcpp/Collection.h"

namespace arangodb {

namespace dbinterface {

Collection::Collection(const Database::SPtr& db, const std::string& id)
    : _database(db), _name(id) {}

Collection::Collection(const Database::SPtr& db, std::string&& id)
    : _database(db), _name(id) {}

Collection::~Collection() {}

namespace {

std::string httpDocApi{"/_api/document"};
std::string httpColApi{"/_api/collection"};
}

ConnectionBase::Url Collection::refColUrl() const {
  return _database->databaseUrl(httpDocApi + '/' + _name);
}

//
// Creates the base url required to get and delete a Document
//
ConnectionBase::Url Collection::refDocUrl(const std::string& key) const {
  return _database->databaseUrl(httpDocApi + '/' + _name + '/' + key);
}

const ConnectionBase::Url Collection::httpApi() const {
  return _database->databaseUrl(httpColApi);
}

const ConnectionBase::Url Collection::httpApiName() const {
  return _database->databaseUrl(httpColApi + '/' + _name);
}

void Collection::addNameAttrib(arangodb::velocypack::Builder& builder) {
  builder.add("name", arangodb::velocypack::Value(_name));
}

bool Collection::hasValidHost() const { return _database->hasValidHost(); }

//
// Now configured for ArangoDB v3.0.x
//
void Collection::docs(const ConnectionBase::SPtr& pCon, const Options opts) {
  typedef Options::List List;
  ConnectionBase& conn = pCon->reset();
  ConnectionBase::Url url{_database->databaseUrl("/_api/simple/all-keys")};
  std::string body{"{\"collection\":\"" + _name + '"'};
  switch (opts.flag<List>()) {
    case List::Id: {
      body += ",\"type\":\"id\"}";
      break;
    }
    case List::Key: {
      body += ",\"type\":\"key\"}";
      break;
    }
    case List::Path:
    default: { body += '}'; }
  }
  conn.setPutReq();
  conn.setPostField(body);
  conn.setUrl(url);
  conn.setBuffer();
}

//
// Configure to create an empty Collection using the configured
// Database.
//
void Collection::create(const Database::SPtr& pDb,
                        const ConnectionBase::SPtr& pCon,
                        const ConnectionBase::VPack& config) {
  ConnectionBase& conn = pCon->reset();
  conn.setUrl(pDb->databaseUrl(httpColApi));
  conn.setPostField(config);
  conn.setBuffer();
}

//
// Configure to create an empty Collection using the configured
// Database and Collection name
//
void Collection::create(const ConnectionBase::SPtr& pCon) {
  ConnectionBase& conn = pCon->reset();
  conn.setUrl(httpApi());
  conn.setPostField("{ \"name\":\"" + _name + "\" }");
  conn.setBuffer();
}

//
// Configure to create an empty Collection using the configured
// Database and Collection name
//
void Collection::collections(const ConnectionBase::SPtr& pCon,
                             const Options opts) {
  ConnectionBase::Url url = httpApi();
  ConnectionBase& conn = pCon->reset();
  if (opts.flagged(Options::ExcludeSystem::True)) {
    conn.addQuery(ConOption{"excludeSystem", "true"});
  }
  conn.setUrl(url);
  conn.setGetReq();
  conn.setBuffer();
}

//
// Configure to delete a Collection using the configured Database
// and Collection name
//
void Collection::remove(const ConnectionBase::SPtr& pCon, const Options  // opts
                        ) {
  ConnectionBase& conn = pCon->reset();
  conn.setDeleteReq();
  conn.setUrl(httpApiName());
  conn.setBuffer();
}

void Collection::about(const ConnectionBase::SPtr& pCon, const Options  // opts
                       ) {
  ConnectionBase& conn = pCon->reset();
  conn.setGetReq();
  conn.setUrl(httpApiName());
  conn.setBuffer();
}

void Collection::rename(const ConnectionBase::SPtr& pCon,
                        const std::string& name,
                        const Options  // opts
                        ) {
  ConnectionBase& conn = pCon->reset();
  std::string data{"{ \"name\":\"" + name + "\" }"};
  conn.setPutReq();
  conn.setUrl(httpApiName() + "/rename");
  conn.setPostField(data);
  conn.setBuffer();
}

//
// Configure to get info for a Collection using the configured
// Database and Collection name
//
void Collection::httpInfo(const ConnectionBase::SPtr& pCon,
                          const Options  // opts
                          ,
                          const std::string&& info) {
  ConnectionBase& conn = pCon->reset();
  conn.setUrl(httpApiName() + info);
  conn.setBuffer();
}

void Collection::checksum(const ConnectionBase::SPtr& pCon,
                          const Options opts) {
  ConnectionBase& conn = pCon->reset();
  ConnectionBase::Url url = httpApiName() + "/checksum";
  if (opts.flagged(Options::Revs::Yes)) {
    conn.addQuery(ConOption("withRevisions", "true"));
  }
  if (opts.flagged(Options::Data::Yes)) {
    conn.addQuery(ConOption("withData", "true"));
  }
  conn.setUrl(url);
  conn.setBuffer();
}

void Collection::truncate(const ConnectionBase::SPtr& pCon,
                          const Options  // opts
                          ) {
  ConnectionBase& conn = pCon->reset();
  conn.setPutReq();
  conn.setUrl(httpApiName() + "/truncate");
  conn.setBuffer();
}
}
}
