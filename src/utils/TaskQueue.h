#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <deque>
#include <functional>
#include <mutex>

enum class Task {
	NotDuringRound,
	MainThread,
};

namespace TaskQueue {
	void add(Task queue_nr, std::function<void()> func);
	void perform(Task queue_nr);
}

#endif
