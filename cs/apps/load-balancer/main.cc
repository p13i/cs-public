// cs/apps/load-balancer/main.cc
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "cs/net/load/balancer/protos/config.proto.hh"
#include "cs/net/load/balancer/protos/gencode/config.validate.hh"
#include "cs/net/load/balancer/server.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"

namespace {
using ::cs::Result;
using ::cs::net::load::balancer::Server;
using ::cs::net::load::balancer::protos::Config;
using ::cs::net::load::balancer::protos::gencode::config::
    validation_generated::ConfigRules;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
}  // namespace

Result RunLoadBalancer(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));

  OK_OR_RET(Validate(config, ConfigRules{}));

  return Server(config.host, config.port,
                config.environment);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunLoadBalancer);
}
