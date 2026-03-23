#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <fstream>
using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
};

// FCFS Scheduling
double calculateFCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    double totalWaitingTime = 0;

    for (auto p : processes) {
        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }

        int waitingTime = currentTime - p.arrivalTime;
        totalWaitingTime += waitingTime;
        currentTime += p.burstTime;
    }

    return totalWaitingTime / processes.size();
}

// SJF Scheduling (Non-preemptive)
double calculateSJF(vector<Process> processes) {
    int n = processes.size();
    vector<bool> completed(n, false);

    int currentTime = 0;
    int finished = 0;
    double totalWaitingTime = 0;

    while (finished < n) {
        int shortestIndex = -1;
        int shortestBurst = 1000000;

        for (int i = 0; i < n; i++) {
            if (!completed[i] &&
                processes[i].arrivalTime <= currentTime &&
                processes[i].burstTime < shortestBurst) {
                shortestBurst = processes[i].burstTime;
                shortestIndex = i;
            }
        }

        if (shortestIndex == -1) {
            currentTime++;
            continue;
        }

        int waitingTime = currentTime - processes[shortestIndex].arrivalTime;
        totalWaitingTime += waitingTime;

        currentTime += processes[shortestIndex].burstTime;
        completed[shortestIndex] = true;
        finished++;
    }

    return totalWaitingTime / n;
}

// Round Robin Scheduling
double calculateRoundRobin(vector<Process> processes, int quantum) {
    int n = processes.size();

    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrivalTime < b.arrivalTime;
    });

    vector<int> remainingBurst(n);
    vector<int> completionTime(n, 0);

    for (int i = 0; i < n; i++) {
        remainingBurst[i] = processes[i].burstTime;
    }

    queue<int> readyQueue;
    int currentTime = 0;
    int i = 0;

    if (n > 0) {
        currentTime = processes[0].arrivalTime;
        readyQueue.push(0);
        i = 1;
    }

    while (!readyQueue.empty()) {
        int index = readyQueue.front();
        readyQueue.pop();

        int executionTime = min(quantum, remainingBurst[index]);
        currentTime += executionTime;
        remainingBurst[index] -= executionTime;

        while (i < n && processes[i].arrivalTime <= currentTime) {
            readyQueue.push(i);
            i++;
        }

        if (remainingBurst[index] > 0) {
            readyQueue.push(index);
        } else {
            completionTime[index] = currentTime;
        }

        if (readyQueue.empty() && i < n) {
            currentTime = processes[i].arrivalTime;
            readyQueue.push(i);
            i++;
        }
    }

    double totalWaitingTime = 0;

    for (int j = 0; j < n; j++) {
        int turnaroundTime = completionTime[j] - processes[j].arrivalTime;
        int waitingTime = turnaroundTime - processes[j].burstTime;
        totalWaitingTime += waitingTime;
    }

    return totalWaitingTime / n;
}

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;

        cout << "Enter arrival time for P" << i + 1 << ": ";
        cin >> processes[i].arrivalTime;

        cout << "Enter burst time for P" << i + 1 << ": ";
        cin >> processes[i].burstTime;
    }

    int quantum;
    cout << "Enter time quantum for Round Robin: ";
    cin >> quantum;

    double fcfsAvg = calculateFCFS(processes);
    double sjfAvg = calculateSJF(processes);
    double rrAvg = calculateRoundRobin(processes, quantum);

    cout << "\n===== Results =====" << endl;
    cout << "FCFS Average Waiting Time: " << fcfsAvg << endl;
    cout << "SJF Average Waiting Time: " << sjfAvg << endl;
    cout << "Round Robin Average Waiting Time: " << rrAvg << endl;

    ofstream file("results.csv");
    file << "Algorithm,Average Waiting Time\n";
    file << "FCFS," << fcfsAvg << "\n";
    file << "SJF," << sjfAvg << "\n";
    file << "Round Robin," << rrAvg << "\n";
    file.close();

    cout << "\nResults saved to results.csv" << endl;

    return 0;
}
