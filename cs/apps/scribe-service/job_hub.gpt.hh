// cs/apps/scribe-service/job_hub.gpt.hh
#ifndef CS_APPS_SCRIBE_SERVICE_JOB_HUB_GPT_HH
#define CS_APPS_SCRIBE_SERVICE_JOB_HUB_GPT_HH

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

namespace cs::apps::scribe_service {

using JobCvMap = std::unordered_map<
    std::string, std::unique_ptr<std::condition_variable>>;
using JobQueueMap =
    std::unordered_map<std::string,
                       std::queue<std::string>>;

// Per-job message queue for long-poll: worker pushes chunks
// and completion; clients wait for next message.
class JobHub {
 public:
  // Pushes a CLI output chunk for job_id (broadcast to
  // waiters).
  void PushChunk(const std::string& job_id,
                 const std::string& chunk);

  // Pushes completion for job_id (status +
  // transcript_text).
  void PushComplete(
      const std::string& job_id,
      const std::string& transcript_text,
      const std::string& status = "completed");

  // Blocks until a message is available for job_id or
  // timeout_sec. Returns JSON message (chunk or completion)
  // or nullopt on timeout.
  std::optional<std::string> WaitForNext(
      const std::string& job_id, int timeout_sec);

 private:
  std::mutex mu_;
  JobQueueMap queues_;
  JobCvMap cvs_;
};

}  // namespace cs::apps::scribe_service

#endif  // CS_APPS_SCRIBE_SERVICE_JOB_HUB_GPT_HH
