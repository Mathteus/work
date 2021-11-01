#include <thread>
#include <mutex>
#include <memory>
#include <utility>
#include <vector>
using namespace std;

class SThread {
    private:
        unsigned short threads_hardware, threads_use;
        vector<thread> threads;

    public:
        SThread(thread&& t){
            threads_hardware = thread::hardware_concurrency();
            threads_use = --threads_hardware;
            for(size_t a=0; a<threads_use; a++){
                threads.push_back(move(t));
            }
        }

        ~SThread(){
            for(size_t a=0; a<threads_use; a++){
                if(threads[a].joinable())
                    threads[a].join();
            }
        }

        unsigned short getNumeroThreads(){ return threads_use; }
};