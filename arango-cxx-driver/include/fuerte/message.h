#pragma once

#include "common_types.h"

#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <map>


namespace arangodb { namespace fuerte { inline namespace v1 {

  // mabye get rid of optional
  struct MessageHeader {
    MessageHeader(MessageHeader const&) = default;
    MessageHeader() = default;
    MessageHeader(MessageHeader&&) = default;

    ::boost::optional<int> version;
    ::boost::optional<MessageType> type;
    ::boost::optional<unsigned> responseCode;
    ::boost::optional<std::string> database;
    ::boost::optional<RestVerb> requestType;
    ::boost::optional<std::string> requestPath;
    ::boost::optional<mapss> parameter;
    ::boost::optional<mapss> meta;
    ::boost::optional<std::string> user;
    ::boost::optional<std::string> password;
    ::boost::optional<ContentType> contentType;
  };

  inline std::string headerToHttp(MessageHeader const& header);
  inline VBuffer headerToVst(MessageHeader const& header);
  inline MessageHeader headerFromHttp(std::string const& body);
  inline MessageHeader headerFromSlice(VSlice const& header_slice);


  // TODO SPLIT MESSAGE INTO REQEST / RESPONSE
  class Message {
    public:
      Message(MessageHeader&& messageHeader = MessageHeader()
             ,mapss&& headerStrings = mapss()
             )
             :header(std::move(messageHeader))
             ,headerStrings(std::move(headerStrings))
             {}
      MessageHeader header;
      mapss headerStrings;
      std::vector<VBuffer> payload;
      uint64_t messageid; //generate by some singleton

      void addPayload(VSlice const& slice){
        VBuffer buffer;
        buffer.append(slice.start(), slice.byteSize());
        payload.push_back(std::move(buffer));
      }

      ContentType contentType(){ return header.contentType.get(); }
  };

  class Request : public Message {
    public:
      Request(MessageHeader&& messageHeader = MessageHeader()
             ,mapss&& headerStrings = mapss()
             ): Message(std::move(messageHeader), std::move(headerStrings))
             {}
  };

  class Response : public Message {
    public:
      Response(MessageHeader&& messageHeader = MessageHeader()
             ,mapss&& headerStrings = mapss()
             ): Message(std::move(messageHeader), std::move(headerStrings))
             {}

  };

  NetBuffer toNetworkVst(Message const&);
  NetBuffer toNetworkHttp(Message const&);

  //boost::optional<Message> fromNetworkVst(NetBuffer const&);
  //boost::optional<Message> fromNetworkHttp(NetBuffer const&);
}}}