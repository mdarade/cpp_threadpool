#pragma once

using namespace std;

typedef std::function<void (void )> fnptr;

class Threadpool {
	public:
		Threadpool(string name, uint32_t n_threads); 
		~Threadpool(); //TBD join all threads
		void CreateThreads();
		void Execute(fnptr );
		void EventLoop();
		string GetName() { return name_; }
		
	private:
		string name_;
		uint32_t n_threads_;
		std::queue<fnptr> q_;
		std::mutex mutex_;
		std::condition_variable cv_;
		std::vector<std::thread> threads_;
		bool running_{true};
};
