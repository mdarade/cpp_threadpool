#include <thread>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>


#include "Threadpool.hpp"


void Threadpool::CreateThreads() {
	int i=0;
	while (i < n_threads_) {
		std::thread(EventLoop);
		i++;
	}
}

Threadpool::Threadpool(string name, uint32_t n_threads){
       	name_ = name;
	n_threads_ = n_threads;
	cout << "hd cores:" << n_threads_ << endl;
	//1. create n_threads
	//2. wait in event loop
	CreateThreads();
	EventLoop();
}


void Threadpool::Execute(std::function<void (void *)> fn) {
	std::unique_lock<std::mutex> lock(mutex_);
	q_.push(std::move(fn));
	lock.unlock();
	cv_.notify_all();
}


void Threadpool::EventLoop() {
	
	cout << "in eventloop" << endl;
	std::unique_lock<std::mutex> lock(mutex_);
	while (true) {
		if (not q_.size()) {
			cv_.wait(lock, [this]{ return (q_.size()!=0); });
		}

		if (q_.size()) {
			cout << "popped ele from q" << endl;
			auto fn = q_.front();
			q_.pop();
			fn;
			lock.unlock();
		}
	}
}


void PrintTime(void* data) {
	cout << "inside " << __func__;
	cout << data;
	auto time_point = chrono::high_resolution_clock::now();
	cout << time_point.time_since_epoch().count() << endl; 
}


int main(int argc, char* argv[]) {
	Threadpool tp("default", std::thread::hardware_concurrency());
	tp.Execute(PrintTime);
	return 0;
}

