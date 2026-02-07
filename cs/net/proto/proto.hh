// cs/net/proto/proto.hh
#ifndef CS_NET_PROTO_PROTO_HH
#define CS_NET_PROTO_PROTO_HH

#include <string>
#include <vector>

#include "cs/net/json/object.hh"
#include "cs/result.hh"

// Forward declaration to avoid circular dependency
namespace cs::net::proto::protos {
struct ProtoMeta;
}

#define DECLARE_PROTO(name_) \
  struct name_ : public cs::net::proto::Proto<name_>

namespace cs::net::proto {

// template <typename T>
// class ProtoMeta {
// public:
// virtual std::vector<std::string>> Fields();
// };

template <typename T>
class ProtoBuilder {
 public:
  T Build();

 protected:
  T instance_;
};

template <typename T>
class Proto {
 public:
  // external/emscripten_bin_linux/emscripten/cache/sysroot/include/c++/v1/__memory/allocator.h:173:9:
  // error: destructor called on non-final
  // 'cs::apps::trycopilotai::protos::Frame' that has
  // virtual functions but non-virtual destructor
  // [-Werror,-Wdelete-non-abstract-non-virtual-dtor]
  virtual ~Proto() = default;
  virtual std::string Serialize(
      unsigned int indent = 0) const;
  virtual cs::ResultOr<T> Parse(std::string s);
  virtual cs::net::json::Object ToObject() const;
  virtual cs::ResultOr<T> FromObject(
      cs::net::json::Object obj) const;
  virtual ProtoBuilder<T> Builder();
  virtual cs::net::proto::protos::ProtoMeta Meta() const;
};
}  // namespace cs::net::proto

#endif  // #define CS_NET_PROTO_PROTO_HH
