#include "TaskQueue.h"

namespace TaskQueue {

	namespace {
		std::mutex queueMutex;
		std::deque<std::deque<std::function<void()>>> queues;
	}

	void pushTask(uint8_t queue_nr, std::function<void()> func) {
		std::lock_guard<std::mutex> mute(queueMutex);

		if (queue_nr >= queues.size())
			queues.resize(queue_nr+1);

		queues[queue_nr].emplace_back(std::move(func));
	}

	void performTasks(uint8_t queue_nr) {
		std::lock_guard<std::mutex> mute(queueMutex);

		for (auto& task : queues[queue_nr])
			task();

		queues[queue_nr].clear();
	}

}