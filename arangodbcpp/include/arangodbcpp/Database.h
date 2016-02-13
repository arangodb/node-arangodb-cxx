////////////////////////////////////////////////////////////////////////////////
/// @brief C++ Library to interface to Arangodb.
///
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
/// @author Copyright 2016, ArangoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////
#ifndef DATABASE_H
#define DATABASE_H

#include <memory>

#include "arangodbcpp/Connection.h"

namespace arangodb
{

namespace dbinterface
{

class Server;

class Database
{
	public:
		typedef std::shared_ptr<Database> SPtr;
		Database() = delete;
		explicit Database(std::shared_ptr<Server> srv, std::string name = "_system");
		~Database();
		void httpCreate(Connection::SPtr conn, bool bAsync = false);
		Connection::VPack httpCreate(bool bSort, Connection::SPtr conn);
		void httpDelete(Connection::SPtr conn, bool bAsync = false);
		Connection::VPack httpDelete(bool bSort, Connection::SPtr conn);
		std::string databaseUrl() const;
		bool hasValidHost() const;

	private:
		std::shared_ptr<Server> _server;
		std::string _name;
};

inline Database::~Database() {}

inline Connection::VPack Database::httpCreate(bool bSort,
		Connection::SPtr conn)
{
	return conn->fromJSon(bSort);
}

inline Connection::VPack Database::httpDelete(bool bSort,
		Connection::SPtr conn)
{
	return conn->fromJSon(bSort);
}

inline bool Database::hasValidHost() const
{
	return _server.get() != nullptr;
}
}
}

#endif  // DATABASE_H