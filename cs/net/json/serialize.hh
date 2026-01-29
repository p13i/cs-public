// cs/net/json/serialize.hh
#ifndef CS_NET_JSON_SERIALIZE_HH
#define CS_NET_JSON_SERIALIZE_HH

#include <iostream>

#include "cs/net/json/object.hh"

namespace cs::net::json {
std::ostream& operator<<(std::ostream& os,
                         const Object& object);

std::ostream& SerializeObject(std::ostream& os,
                              const Object& object);

std::string SerializeObject(const Object& object,
                            unsigned int indent);

std::ostream& SerializeObjectPrettyPrintRecurse(
    std::ostream& os, const Object& object,
    unsigned int indent, unsigned int initial_indent);

}  // namespace cs::net::json

#endif  // CS_NET_JSON_SERIALIZE_HH