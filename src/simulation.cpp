#include "simulation.h"
#include "types/event.h"
#include "types/process.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include "types/system_stats.h"

using namespace std;


void Simulation::run(const string& filename) {
  read_file(filename);

  // While their are still events to process, invoke the corresponding methods
  // to handle them.
  while (!events.empty()) {
    const Event* event = events.top();
    events.pop();

    // Invoke the appropriate method on the scheduler for the given event type.
    switch (event->type) {
    case Event::THREAD_ARRIVED:
      handle_thread_arrived(event);
      break;

    case Event::THREAD_DISPATCH_COMPLETED:
      handle_thread_dispatch_completed(event);
      break;

    case Event::PROCESS_DISPATCH_COMPLETED:
      handle_process_dispatch_completed(event);
      break;

    case Event::CPU_BURST_COMPLETED:
      handle_cpu_burst_completed(event);
      break;

    case Event::IO_BURST_COMPLETED:
      handle_io_burst_completed(event);
      break;

    case Event::THREAD_COMPLETED:
      handle_thread_completed(event);
      break;

    case Event::THREAD_PREEMPTED:
      handle_thread_preempted(event);
      break;

    case Event::DISPATCHER_INVOKED:
      handle_dispatcher_invoked(event);
      break;
    }

    // Free the event's memory.
    delete event;
  }
   for(unsigned int i = 0; i < processes.size(); i++){
	logger.print_process_details(processes[i]);
  }
  stats = calculate_statistics();
  logger.print_statistics(stats);
}


//==============================================================================
// Event-handling methods
//==============================================================================

/*string Simulation::type_string(Process::Type t){
//Process::Type t = event->thread->process->type;
string temp = "";
  switch(t){
	case Process::Type::SYSTEM:
		temp = "SYSTEM";
		break;
	case Process::Type::INTERACTIVE:
		temp = "INTERACTIVE";
		break;
	case Process::Type::NORMAL:
		temp = "NORMAL";
		break;
	case Process::Type::BATCH:
		temp = "BATCH";
		break;
  }  

return temp;
}*/

/*string Simulation::state_string(Thread::State s){
//Process::Type t = event->thread->process->type;
string temp = "";
  switch(s){
	case Thread::State::NEW:
		temp = "NEW";
		break;
	case Thread::State::READY:
		temp = "READY";
		break;
	case Thread::State::RUNNING:
		temp = "RUNNING";
		break;
	case Thread::State::BLOCKED:
		temp = "BLOCKED";
		break;
	case Thread::State::EXIT:
		temp = "EXIT";
		break;
  }  

return temp;
}*/


void Simulation::handle_thread_arrived(const Event* event) {
  //if(start==-1){start = event->time;}
  //Set Ready
  //Then enqueue
 // event->thread->arrival_time = time;
  //event->thread->set_ready(event->time);
 assert(event->thread->current_state == Thread::State::NEW);
if(event->thread->current_state == Thread::State::NEW){
	event->thread->current_state = Thread::State::READY;
	event->thread->previous_state = Thread::State::NEW;
}
assert(event->thread->current_state == Thread::State::READY);
assert(event->thread->previous_state == Thread::State::NEW);

 scheduler->enqueue(event, event->thread);
  
event->thread->start_time = event->time;

  //active_thread is the thread that is currently executing
  //events is a queue containing objects of event type
  if(!active_thread){
  	//if we are here, then the processor is idle --> move to DISPATCHER INVOKED
  	//using constructor works!
  	events.push(new Event(Event::DISPATCHER_INVOKED, event->time /*+ event->thread->bursts.front()->length*/, event->thread, NULL));
  }

//  cout << "At time " << event->time << ":" << endl;  

  //cout << "\tTHREAD_ARRIVED" << endl;

logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}

void Simulation::handle_thread_dispatch_completed(const Event* event) {
  // Set RUNNING
  //Set last thread = current thread
  //int burst_length = event->thread->set_running(event->time);
  size_t time_slice = event->scheduling_decision->time_slice;
  Burst* bl = event->thread->bursts.front();

  prev_thread = active_thread;
 // active_thread = NULL;
  //See if time slice is less than burst time
  //if yes, move to Thread Preempted
  //if no, move to CPU burst completed

if(event->thread->previous_state == Thread::State::NEW){
	event->thread->state_change_time = event->time;
}

assert(event->thread->current_state == Thread::State::READY);
event->thread->previous_state = Thread::State::READY;
event->thread->current_state = Thread::State::RUNNING;
assert(event->thread->current_state == Thread::State::RUNNING);
assert(event->thread->previous_state == Thread::State::READY);


//  if(time_slice < burst_length){
//	events.push(new Event(Event::THREAD_PREEMPTED, event->time + time_slice, event->thread, NULL));
 // }else{
//	events.push(new Event(Event::CPU_BURST_COMPLETED, event->time + burst_length, event->thread, NULL));
 // }

if(time_slice < (unsigned)bl->length){
	bl->length -= event->scheduling_decision->time_slice;
	event->thread->service += event->scheduling_decision->time_slice;
	events.push(new Event(Event::THREAD_PREEMPTED, event->time + time_slice, event->thread, NULL));
}else{
	events.push(new Event(Event::CPU_BURST_COMPLETED, event->time + bl->length, event->thread, NULL));
}

 //  cout << "At time " << event->time << ":" << endl;  
   //cout << "\tTHREAD_DISPATCH_COMPLETED" << endl;
   
logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_process_dispatch_completed(const Event* event) {
  // Set RUNNING
  //Set last thread = current thread
  //int burst_length = event->thread->set_running(event->time);
  /*int time_slice = event->scheduling_decision->time_slice;

  prev_thread = active_thread;
  active_thread = NULL;

  if(event->thread->previous_state==Thread::State::NEW){
	event->thread->state_change_time = event->time;
}

event->thread->previous_state = Thread::State::READY;
event->thread->current_state = Thread::State::RUNNING;

  //See if time slice is less than burst time
  //if yes, move to Thread Preempted
  //if no, move to CPU burst completed
   if(time_slice < burst_length){
	events.push(new Event(Event::THREAD_PREEMPTED, event->time + time_slice, event->thread, NULL));
  }else{
	events.push(new Event(Event::CPU_BURST_COMPLETED, event->time + burst_length, event->thread, NULL));
  }*/

handle_thread_dispatch_completed(event);

//  cout << "At time " << event->time << ":" << endl;  
  //cout << "\tPROCESS_DISPATCH_COMPLETED" << endl;

logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_cpu_burst_completed(const Event* event) {
  // Pop burst, unset current thread
  //Last CPU Burst?
  //If yes, thread completed --> Set exit!
  //If no, Move to IO burst completed, set Blocked
  //int burst_length = event->thread->set_blocked(event->time);

if(event->thread->bursts.front()->type == Burst::Type::CPU){
	event->thread->service += event->thread->bursts.front()->length;
}else{
	event->thread->io_time += event->thread->bursts.front()->length;
}
event->thread->bursts.pop();



 // event->thread->pop_burst(event->time);


//  if(burst_length==-1){
//	events.push(new Event(Event::THREAD_COMPLETED, event->time/* + event->thread->bursts.front()->length*/, event->thread, NULL));
//	return;
 // }

  prev_thread = active_thread;
  active_thread = NULL;

//  events.push(new Event(Event::IO_BURST_COMPLETED, event->time + burst_length, event->thread, NULL));

  //if(!active_thread){
//	events.push(new Event(Event::DISPATCHER_INVOKED, event->time, NULL, NULL));
  //}

if(event->thread->bursts.size() > 0){
	assert(event->thread->current_state == Thread::State::RUNNING);
	event->thread->previous_state = Thread::State::RUNNING;
	event->thread->current_state = Thread::State::BLOCKED;
	assert(event->thread->current_state == Thread::State::BLOCKED);
	assert(event->thread->previous_state == Thread::State::RUNNING);

	int bl = event->thread->bursts.front()->length;
	events.push(new Event(Event::IO_BURST_COMPLETED, event->time + bl, event->thread, NULL));
}else{
	events.push(new Event(Event::THREAD_COMPLETED, event->time, event->thread, NULL));
}

events.push(new Event(Event::DISPATCHER_INVOKED, event->time, event->thread, NULL));

//  cout << "At time " << event->time << ":" << endl;  
  //cout << "\tCPU_BURST_COMPLETED" << endl;

logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_io_burst_completed(const Event* event) {
  //Set READY
  //Enqueue and pop burst
  //event->thread->set_ready(event->time);
assert(event->thread->current_state == Thread::State::BLOCKED);
event->thread->previous_state = Thread::State::BLOCKED;
event->thread->current_state = Thread::State::READY;  
assert(event->thread->current_state = Thread::State::READY);
assert(event->thread->previous_state = Thread::State::BLOCKED);

scheduler->enqueue(event, event->thread);
  //event->thread->pop_burst(event->time);

  //Go back to checking if processor is idle
 // if(!active_thread){
//	events.push(new Event(Event::DISPATCHER_INVOKED, event->time/* + event->thread->bursts.front()->length*/, NULL, NULL));
 // }

if(event->thread->bursts.front()->type == Burst::Type::CPU){
	event->thread->service += event->thread->bursts.front()->length;
}else{
	event->thread->io_time += event->thread->bursts.front()->length;
}
event->thread->bursts.pop();

if(!active_thread){
	events.push(new Event(Event::DISPATCHER_INVOKED, event->time, event->thread, NULL));
}

//  cout << "At time " << event->time << ":" << endl;  
  //cout << "\tIO_BURST_COMPLETED" << endl;
 
logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_thread_completed(const Event* event) {
  // Set exit
  //end = event->time;
  //event->thread->set_exit(event->time);
assert(event->thread->current_state == Thread::State::RUNNING);
event->thread->previous_state = Thread::State::RUNNING;
event->thread->current_state = Thread::State::EXIT;
assert(event->thread->current_state == Thread::State::EXIT);
assert(event->thread->previous_state == Thread::State::RUNNING);

 // prev_thread = active_thread;
 // active_thread = NULL;

  //if(!active_thread){
//	events.push(new Event(Event::DISPATCHER_INVOKED, event->time/* + event->thread->bursts.front()->length*/, NULL, NULL));
 // }

stats.total_time = event->time;
event->thread->end_time = event->time;

//  cout << "At time " << event->time << ":" << endl;  
  //cout << "\tTHREAD_COMPLETED" << endl;

logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_thread_preempted(const Event* event) {
  // Set READY
  //Enqueue
  //Decrease CPU Burst
  //event->thread->set_ready(event->time);
  assert(event->thread->current_state == Thread::State::RUNNING);
event->thread->previous_state = Thread::State::RUNNING;
event->thread->current_state = Thread::State::READY;
assert(event->thread->current_state == Thread::State::READY);
assert(event->thread->previous_state == Thread::State::RUNNING);

scheduler->enqueue(event, event->thread);
 // prev_thread = active_thread;
  active_thread = NULL;


  
  //Go back to invoking dispatcher
  if(!active_thread){
	events.push(new Event(Event::DISPATCHER_INVOKED, event->time /*+ event->thread->bursts.front()->length*/, event->thread, NULL));
  }
 // cout << "At time " << event->time << ":" << endl;  
  //cout << "\tTHREAD_PREEMPTED" << endl;

logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
}


void Simulation::handle_dispatcher_invoked(const Event* event) {
  //Get scheduling decision and set current thread
  SchedulingDecision* dec = new SchedulingDecision();
dec = scheduler->get_next_thread(event);
if(!event){return;} 
  //logger.print_verbose(event, event->thread, dec->explanation);
  if(!dec){ return;}
  if(!(dec->thread)){
	return; 
  }
 logger.print_verbose(event, event->thread, dec->explanation);

  //active_thread = dec->thread;
  
  //Decision made?
  //See if Process or Thread Switch
  //If Thread, move to Thread Dispatch
  //If process, move to Process Dispatch complete


  //soooo if current and previous threads are not null and both threads belong to the same process
 // if(active_thread && prev_thread && (prev_thread->process == active_thread->process)){
	//Thread dispatch is complete
//	events.push(new Event(Event::THREAD_DISPATCH_COMPLETED, event->time + thread_switch_overhead, dec->thread, dec));
//	dispatch += thread_switch_overhead; 
  //}else{
	//Process dispatch is complete
//	events.push(new Event(Event::PROCESS_DISPATCH_COMPLETED, event->time + process_switch_overhead, dec->thread, dec));
//	dispatch += process_switch_overhead; 

  //}

if(prev_thread != NULL){
	if(prev_thread->process != dec->thread->process){
		events.push(new Event(Event::PROCESS_DISPATCH_COMPLETED, event->time + process_switch_overhead, dec->thread, dec));
		stats.dispatch_time += process_switch_overhead;
	}else{
		events.push(new Event(Event::THREAD_DISPATCH_COMPLETED, event->time + thread_switch_overhead, dec->thread, dec));
		stats.dispatch_time += thread_switch_overhead;
	}
}else{
	events.push(new Event(Event::PROCESS_DISPATCH_COMPLETED, event->time + process_switch_overhead, dec->thread, dec));
	stats.dispatch_time += process_switch_overhead;
}

active_thread = dec->thread;

//  cout << "At time " << event->time << ":" << endl;  
 // cout << "\tDISPATCHER_INVOKED" << endl;

//logger.print_state_transition(event, active_thread->previous_state, active_thread->current_state);
}


//==============================================================================
// Utility methods
//==============================================================================


void Simulation::add_event(Event* event) {
  if (event != nullptr) {
    events.push(event);
  }
}


void Simulation::read_file(const string& filename) {
  ifstream file(filename.c_str());

  if (!file) {
    cerr << "Unable to open simulation file: " << filename << endl;
    exit(EXIT_FAILURE);
  }

  size_t num_processes;

  // Read the total number of processes, as well as the dispatch overheads.
  file >> num_processes >> thread_switch_overhead >> process_switch_overhead;

  // Read in each process.
  for (size_t p = 0; p < num_processes; p++) {
    Process* process = read_process(file);

    processes[process->pid] = process;
  }
}


Process* Simulation::read_process(istream& in) {
  int pid, type;
  size_t num_threads;

  // Read in the process ID, its type, and the number of threads.
  in >> pid >> type >> num_threads;

  // Create the process and register its existence in the processes map.
  Process* process = new Process(pid, (Process::Type) type);

  // Read in each thread in the process.
  for (size_t tid = 0; tid < num_threads; tid++) {
    process->threads.push_back(read_thread(in, tid, process));
  }

  return process;
}


Thread* Simulation::read_thread(istream& in, int tid, Process* process) {
  int arrival_time;
  size_t num_cpu_bursts;

  // Read in the thread's arrival time and its number of CPU bursts.
  in >> arrival_time >> num_cpu_bursts;

  Thread* thread = new Thread(arrival_time, tid, process);

  // Read in each burst in the thread.
  for (size_t n = 0, burst_length; n < num_cpu_bursts * 2 - 1; n++) {
    in >> burst_length;

    Burst::Type burst_type = (n % 2 == 0)
        ? Burst::CPU
        : Burst::IO;

    thread->bursts.push(new Burst(burst_type, burst_length));
  }

  // Add an arrival event for the thread.
  events.push(new Event(Event::THREAD_ARRIVED, thread->arrival_time, thread));

  return thread;
}


SystemStats Simulation::calculate_statistics() {
  SystemStats s = SystemStats();

  for(map <int, Process*>::iterator it = processes.begin(); it != processes.end(); it++){
	Process::Type type = it->second->type;
	const std::vector<Thread*> thds = it->second->threads;

	for(unsigned int i = 0; i < thds.size(); i++){
		int n = ++s.thread_counts[type];
		int temp1 = thds[i]->service;
		s.service_time += temp1;
		s.io_time += thds[i]->io_time;
		s.avg_thread_response_times[type] = s.avg_thread_response_times[type]*(n-1)/n + thds[i]->response_time();
		s.avg_thread_turnaround_times[type] = s.avg_thread_turnaround_times[type]*(n-1)/n + thds[i]->turnaround_time() * 1.0/n;
	}
  }

//total elapsed time
  s.total_time = s.service_time + s.dispatch_time;
//total service time -->above

//total i/o time--> above

//total dispatch time
  s.dispatch_time = dispatch;
//total idle time
  s.total_idle_time = s.total_time - s.total_cpu_time;
//cpu utilization
 // s.cpu_utilization = 100.0*(s.dispatch_time + s.service_time)/s.total_time;
s.cpu_utilization = 100.0*s.total_cpu_time / s.total_time;
//cpu efficiency
  s.cpu_efficiency = 100.0*s.service_time/s.total_time;
  return s;
}
