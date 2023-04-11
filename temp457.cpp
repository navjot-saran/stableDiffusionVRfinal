// Navjot Saran
// 30011914
// A4 CPSC457
// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include "common.h"
#include <algorithm>
#include <deque>

#include <iostream>

using namespace std;

// this is the function you should implement
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//

void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) {
    // replace the wrong implementation below with your own!!!!
    seq.clear();
   
    int64_t time = 0;std::vector<Process> processes_copy(processes);
    
    std::deque<Process> job_queue;
    std::vector<int> ready_queue;
    // bool p_finished = false;
    bool all_finished = false;
    
    std::vector<Process> arrived_processes;
    arrived_processes.push_back(processes[0]);

    while(arrived_processes.back().arrival > time) {
        seq.push_back(-1);
        time = time + 1;
    }

    arrived_processes.clear();

    std::vector<Process>::size_type proc_index = 0;
    // std::vector<int64_t> remaining_bursts;
    cout << "arrived proc: " << processes[0].arrival << endl;

    // std::vector<Process>::size_type proc_index = 0;
    // std::vector<Process>::size_type arrived_proc_index = 0;
    cout << "while at 65" << endl;
    while(!processes_copy.empty()) {
        std::vector<Process>::size_type arrived_proc_index = 0;
        while ((proc_index < processes.size()) && (processes[proc_index].arrival <= time)) {
            arrived_processes.push_back(processes[proc_index]);
            proc_index++;
            if (processes[proc_index].arrival == processes[proc_index-1].arrival) {
                reverse(arrived_processes.begin(), arrived_processes.end());
                continue;
            }
            if (processes[proc_index].arrival > time) {
                continue;
            }
            cout << "proc index: " << proc_index << endl;
            cout << "arr proc id: " << processes[proc_index].id << endl;
            cout << "arrived proc: " << processes[proc_index].arrival << endl;
            cout << "time: " << time << endl;
        }
        if (!arrived_processes.empty()){
            cout << "while at 73" << endl;
            cout << "733333arrived proc: " << arrived_processes[arrived_proc_index].arrival << endl;
            cout << "733333arrived back: " << arrived_processes.back().arrival << endl;
            while (arrived_processes[arrived_proc_index].arrival <= arrived_processes.back().arrival) {
                job_queue.push_front(arrived_processes[arrived_proc_index]);
                arrived_proc_index++;
                // if (job_queue.size() > 1) {
                //     job_queue.pop_back();
                // }
                // job_queue.pop_back();
                // cout << "job queue: " << job_queue[0].id << " " << job_queue[1].id << " " << job_queue[2].id << " " << job_queue[3].id << " " << job_queue[4].id << " " << endl;
                for (auto & p : job_queue) {
                    cout << "job queue: " << p.id << endl;
                }
                cout << " " << endl;
            }
        }

        arrived_processes.clear();
        cout << "for at 90" << endl;
        for (auto & process : job_queue) {
            if (process.burst > 0) {
                ready_queue.push_back(process.id);
            }
            // ready_queue.push_back(process.id);
            
        }

        for (auto & proc : ready_queue) {
            cout << "inside rq: " << proc << endl;
           
            for (auto & p : job_queue) {
                if (p.id == proc) {
                    cout << "we made it a lil" << endl;
                    // cout << p.id << "pburst: " << p.burst << endl;
                    if (p.burst >= quantum) {
                        p.burst = p.burst - quantum;
                        time = time + quantum;
                        seq.push_back(proc);
                        // if(p.burst == 0) {
                        //     job_queue.push_back(p);
                        //     job_queue.pop_back();
                        // }
                    }
                    else {
                        p.burst = 0;
                        time = time + p.burst;
                        seq.push_back(proc);
                        for (auto & process : seq) {
                            cout << "seq " << process << endl;
                        }
                        if (proc == processes.back().id) {
                            all_finished = true;
                        }
                        cout << " " << endl;
                        // p_finished = true;
                        // job_queue.push_back(p);
                        // job_queue.pop_back();
                        // p.finish_time = time;
                        // p.start_time = p.arrival;
                        cout << "job size: " << job_queue.size() <<  endl;
                    }
                    ready_queue.pop_back();
                    // cout << p.id << "NEWpburst: " << p.burst << endl;
                }
            }
            ready_queue.clear();
        }
        cout << " " << endl;
        // break;
        // if (p_finished == true) {
        //     cout << "AFTER A PROCESS FINISHED" << endl;
        //     for (auto & process : seq) {
        //         cout << "seq " << process << endl;
        //     }
        //     // processes_copy.clear();
        //     // break;
        // }

        if (all_finished == true) {
            processes_copy.clear();
        }

    }

    // for (auto & process : seq) {
    //     cout << "seq " << process << endl;
    // }

    for (const auto& process : job_queue) {
        std::cout << "Process " << process.id << ": " << process.burst << std::endl;
    }


}
