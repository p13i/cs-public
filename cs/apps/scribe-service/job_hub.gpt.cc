// cs/apps/scribe-service/job_hub.gpt.cc
#include "cs/apps/scribe-service/job_hub.gpt.hh"

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <string>

namespace cs::apps::scribe_service {

void JobHub::PushChunk(const std::string& job_id,
                       const std::string& chunk) {
  std::lock_guard<std::mutex> lock(mu_);
  queues_[job_id].push(chunk);
  auto it = cvs_.find(job_id);
  if (it != cvs_.end() && it->second) {
    it->second->notify_all();
  }
}

void JobHub::PushComplete(
    const std::string& job_id,
    const std::string& transcript_text,
    const std::string& status) {
  std::ostringstream json;
  json << "{\"status\":\"" << status
       << "\",\"transcript_text\":\"";
  for (char c : transcript_text) {
    if (c == '"')
      json << "\\\"";
    else if (c == '\\')
      json << "\\\\";
    else if (c == '\n')
      json << "\\n";
    else if (c == '\r')
      json << "\\r";
    else
      json << c;
  }
  json << "\"}";
  PushChunk(job_id, json.str());
}

std::optional<std::string> JobHub::WaitForNext(
    const std::string& job_id, int timeout_sec) {
  std::unique_lock<std::mutex> lock(mu_);
  auto& q = queues_[job_id];
  if (cvs_.find(job_id) == cvs_.end()) {
    cvs_[job_id] =
        std::make_unique<std::condition_variable>();
  }
  auto& cv = *cvs_[job_id];

  auto deadline =
      std::chrono::steady_clock::now() +
      std::chrono::seconds(std::max(1, timeout_sec));
  cv.wait_until(lock, deadline,
                [&q] { return !q.empty(); });

  if (q.empty()) {
    return std::nullopt;
  }
  std::string msg = std::move(q.front());
  q.pop();
  return msg;
}

}  // namespace cs::apps::scribe_service
