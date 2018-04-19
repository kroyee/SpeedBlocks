#include "TaskQueue.h"

namespace TaskQueue {

	namespace {
		std::mutex queueMutex;
		std::deque<std::deque<std::function<void()>>> queues;
	}

	void add(Task queue_id, std::function<void()> func) {
		auto queue_nr = static_cast<uint8_t>(queue_id);
		std::lock_guard<std::mutex> mute(queueMutex);

		if (queue_nr >= queues.size())
			queues.resize(queue_nr+1);

		queues[queue_nr].emplace_back(std::move(func));
	}

	void perform(Task queue_id) {
		auto queue_nr = static_cast<uint8_t>(queue_id);
		std::lock_guard<std::mutex> mute(queueMutex);

		for (auto& task : queues[queue_nr])
			task();

		queues[queue_nr].clear();
	}

}
