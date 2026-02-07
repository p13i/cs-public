// cs/apps/task-queue-service/task_worker.gpt.hh
#ifndef CS_APPS_TASK_QUEUE_SERVICE_TASK_WORKER_GPT_HH
#define CS_APPS_TASK_QUEUE_SERVICE_TASK_WORKER_GPT_HH

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "cs/net/json/object.hh"
#include "cs/result.hh"

namespace cs::apps::task_queue_service {

using TaskHandler =
    std::function<cs::Result(const cs::net::json::Object&)>;

class TaskRegistry {
 public:
  cs::Result RegisterTask(std::string task_name,
                          TaskHandler handler);

  [[nodiscard]]
  bool HasTask(const std::string& task_name) const;

  cs::Result Dispatch(
      const std::string& task_name,
      const cs::net::json::Object& args) const;

 private:
  std::unordered_map<std::string, TaskHandler> handlers_;
};

struct WorkerOptions {
  std::string consumer_service;
  std::string dequeue_endpoint = "/rpc/dequeue/";
  int empty_poll_sleep_ms = 15000;
  std::optional<int> max_iterations;
};

cs::Result RegisterBuiltInTasks(TaskRegistry& registry);

cs::Result RunWorker(
    const std::string& base_url, TaskRegistry& registry,
    const WorkerOptions& options = WorkerOptions());

}  // namespace cs::apps::task_queue_service

#endif  // CS_APPS_TASK_QUEUE_SERVICE_TASK_WORKER_GPT_HH
