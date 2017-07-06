////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017 ArangoDB GmbH, Cologne, Germany
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
/// @author Jan Christoph Uhde
/// @author Ewout Prangsma
////////////////////////////////////////////////////////////////////////////////

#include "node_init.h"
#include "node_connection.h"
#include "node_connection_builder.h"
#include "node_request.h"
#include "node_response.h"
#include "node_vpack.h"

#include <iostream>
#include <fuerte/loop.h>
#include <fuerte/FuerteLogger.h>

namespace arangodb { namespace fuerte { namespace js {

NAN_MODULE_INIT(InitAll) {
  FUERTE_LOG_NODE << "About to init classes" << std::endl;
  InitVPack(target);
  NConnectionBuilder::Init(target);
  NConnection::Init(target);
  NRequest::Init(target);
  NResponse::Init(target);
}

}}}

//
// Names the node and the function call to initialise
// the functionality it will provide
//
NODE_MODULE(fuerte, ::arangodb::fuerte::js::InitAll);
