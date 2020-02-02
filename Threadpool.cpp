#include <thread>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <vector>
#include <sstream>

#include "Threadpool.hpp"


Threadpool::Threadpool(string name, uint32_t n_threads){
       	name_ = name;
	n_threads_ = n_threads;
	threads_.reserve(n_threads_);
	cout << "number of cores:" << n_threads_ << endl;
}


Threadpool::~Threadpool() {

	std::unique_lock<std::mutex> lock(mutex_);
	running_ = false;
	lock.unlock();

	cv_.notify_all();

	uint32_t i = 0;
	while (i < n_threads_) {
		if (threads_[i].joinable()) {
		       threads_[i].join();	
		}
		i++;
	}
}

void Threadpool::CreateThreads() {
	int i=0;
	while (i < n_threads_) {
		threads_[i] = std::thread(&Threadpool::EventLoop, this);
		i++;
	}
}


void Threadpool::Execute(fnptr fn) {
	std::unique_lock<std::mutex> lock(mutex_);
	q_.push(std::move(fn));
	lock.unlock();
	cv_.notify_all();
}


void Threadpool::EventLoop(void) {
	std::unique_lock<std::mutex> lock(mutex_);
	while (running_) {
		cv_.wait(lock, [this]{ return (q_.size() || running_==false); });
		if (q_.size() and running_) {
			auto fn = std::move(q_.front());
			q_.pop();
			lock.unlock();
			fn();
			lock.lock();
		}
	}
}


void PrintTime(void) {
	auto time_point = chrono::high_resolution_clock::now();
	std::stringstream t;
	t << std::this_thread::get_id() << ":::" << time_point.time_since_epoch().count() << endl; 
	cout << t.str();
}


int main(int argc, char* argv[]) {
	Threadpool tp("default", std::thread::hardware_concurrency());
	tp.CreateThreads();
	tp.Execute(PrintTime);
	tp.Execute(PrintTime);
	tp.Execute(PrintTime);
	tp.Execute(PrintTime);
	tp.Execute(PrintTime);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return 0;
}
