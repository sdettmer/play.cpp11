/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/consumer.cc A simple multithreaded consumer/producer with test.
 */

#include "cpp11/consumer.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>

void consumer_test()
{
    using Message=std::string;
    // A simple synchronized message queue, to be used from multiple threads.
    class Queue {
        std::queue<Message> mqueue_;
        std::condition_variable mcond_;
        std::mutex mmutex_;

        public:
        void add(const Message &m) {
            std::cout << "add " << m << std::endl;
            std::unique_lock<std::mutex> lock { mmutex_ };
            mqueue_.push(m);
            mcond_.notify_one();
        }
        Message get() {
            std::unique_lock<std::mutex> lock { mmutex_ };
            if (!mqueue_.empty()) {
                Message m = mqueue_.front();
                mqueue_.pop();
                std::cout << "had " << m << std::endl;
                return m;
            }
            // else: empty --> wait.
            mcond_.wait(lock);
            Message m = mqueue_.front();
            mqueue_.pop();
            std::cout << "waited for " << m << std::endl;
            return m;
        }
    };

    // A simple producer producing messages by the time.
    class Producer {
        Queue &queue_;
        public:
        Producer(Queue &queue) : queue_(queue) { }
        void operator()() {
            queue_.add("MSG_0");
            std::this_thread::sleep_for(std::chrono::milliseconds{1000});
            queue_.add("MSG_1");
            std::this_thread::sleep_for(std::chrono::milliseconds{1000});
            queue_.add("MSG_2a");
            queue_.add("MSG_2b");
            std::this_thread::sleep_for(std::chrono::milliseconds{1000});
            queue_.add("MSG_3a");
            queue_.add("MSG_3b");
            std::this_thread::sleep_for(std::chrono::milliseconds{1000});
            queue_.add("STOP");
            std::cout << "Producer done" << std::endl;
        }
    };

    // A simple consumer consuming available messages.
    class Consumer {
        Queue &queue_;
        public:
        Consumer(Queue &queue) : queue_(queue) { }
        void operator()() {
            for(;;) {
                Message m=queue_.get();
                if (m=="STOP") break;
                std::cout << "consumed " << m << std::endl;
            }
            std::cout << "Consumer done" << std::endl;
        }
    };

    Queue queue;
    // Consumer consumer(queue);
    // Producer producer(queue);

    std::thread producer { Producer(queue) };
    std::thread consumer { Consumer(queue) };
    producer.join();
    consumer.join();
}

/* vim: set ts=4 sw=4 tw=76: */
