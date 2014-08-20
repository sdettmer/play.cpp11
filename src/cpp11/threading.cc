/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/threading.cc A simple multithreading test.
 */
#include "cpp11/threading.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

void threading_test()
{
    // A thread that sleeps for one second.
    struct MyThread {
        const std::string name_;
        MyThread() : name_("") { }
        MyThread(const std::string &name) : name_(name) { }
        void operator()() {
            {
                if (name_ != "") {
                    std::cout << "start " << name_ << std::endl;
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds{1000});
                    std::cout << "done " << name_ << std::endl;
                }
            }
        }
    };

    // Note that std::thread constructor copies "MyThread".
    std::thread t1 { MyThread { "t1" } };
    std::thread t2 { MyThread { "t2" } };

    // Let's lock a mutex:
    {
        std::mutex m;
        std::unique_lock<std::mutex> lock { m };
    }

    // Let's look mulitple mutexes atomically:
    {
        std::mutex m1;
        std::mutex m2;
        std::unique_lock<std::mutex> lock1 { m1, std::defer_lock };
        std::unique_lock<std::mutex> lock2 { m2, std::defer_lock };

        std::lock(lock1, lock2);
    }

    // Wait for both threads to be finished (take approximately
    // one second in total).
    t1.join();
    t2.join();
}

/* vim: set ts=4 sw=4 tw=76: */
