// cs/apps/prober/main.gpt.cc
#include <string>
#include <vector>

#include "cs/apps/prober/prober.gpt.hh"
#include "cs/apps/prober/protos/config.proto.hh"
#include "cs/apps/prober/protos/gencode/config.validate.hh"
#include "cs/apps/prober/protos/probes.proto.hh"
#include "cs/fs/fs.hh"
#include "cs/net/proto/validators.gpt.hh"
#include "cs/parsers/arg_parser.gpt.hh"
#include "cs/result.hh"

namespace {  // use_usings
using ::cs::Result;
using ::cs::apps::prober::RunProbes;
using ::cs::apps::prober::protos::Config;
using ::cs::apps::prober::protos::Probes;
using ::cs::apps::prober::protos::gencode::config::
    validation_generated::ConfigRules;
using ::cs::fs::read;
using ::cs::net::proto::validation::Validate;
using ::cs::parsers::ParseArgs;
}  // namespace

Result RunProber(std::vector<std::string> argv) {
  SET_OR_RET(auto config, ParseArgs<Config>(argv));
  OK_OR_RET(Validate(config, ConfigRules{}));

  SET_OR_RET(std::string contents,
             read(config.probes_file));

  SET_OR_RET(Probes probes, Probes().Parse(contents));

  return RunProbes(config.host, config.port, probes.probes);
}

int main(int argc, char** argv) {
  return Result::Main(argc, argv, RunProber);
}
