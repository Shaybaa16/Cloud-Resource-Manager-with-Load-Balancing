#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>
#include <chrono>
#include <windows.h>
using namespace std;

class Job
{
public:
    bool status;
    int JobID;
    int Priority;
    bool JobStatus;
    int Ram;
    int Processor;
    int RTC;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::chrono::time_point<std::chrono::steady_clock> endTime;

    void setJob(int *arr)
    {
        JobID = arr[0];
        Priority = arr[1];
        Ram = arr[2];
        Processor = arr[3];
        RTC = arr[4];
        DisplayJobDetails();
    }

    Job(int id = 0, int priority = 0, bool jobStatus = false, int ram = 0, int proc = 0, int rtc = 0)
        : JobID(id), Priority(priority), JobStatus(jobStatus), Ram(ram), Processor(proc), RTC(rtc)
    {
        // Initialize other members if needed
    }

    friend ostream &operator<<(ostream &os, const Job &job)
    {
        const int columnWidth = 15;

        os << left << setw(columnWidth) << "Job ID: " << setw(columnWidth) << job.JobID << endl;
        os << left << setw(columnWidth) << "Priority: " << setw(columnWidth) << job.Priority << endl;
        os << left << setw(columnWidth) << "Status: " << setw(columnWidth) << (job.JobStatus ? "Active" : "Inactive") << endl;
        os << left << setw(columnWidth) << "Ram: " << setw(columnWidth - 13) << job.Ram << " GB" << endl;
        os << left << setw(columnWidth) << "Processor: " << setw(columnWidth - 14) << job.Processor << " GHz" << endl;
        os << left << setw(columnWidth) << "RTC: " << setw(columnWidth - 13) << job.RTC << " Hz" << endl;

        return os;
    }

    void DisplayJobDetails()
    {
        const int columnWidth = 15;

        cout << left << setw(columnWidth) << "Job ID: " << setw(columnWidth) << JobID << endl;
        cout << left << setw(columnWidth) << "Priority: " << setw(columnWidth) << Priority << endl;
        cout << left << setw(columnWidth) << "Status: " << setw(columnWidth) << (JobStatus ? "Active" : "Inactive") << endl;
        cout << left << setw(columnWidth) << "Ram: " << setw(columnWidth - 13) << Ram << " GB" << endl;
        cout << left << setw(columnWidth) << "Processor: " << setw(columnWidth - 14) << Processor << " GHz" << endl;
        cout << left << setw(columnWidth) << "RTC: " << setw(columnWidth - 13) << RTC << " Hz" << endl;
    }

    void startExecution()
    {
        startTime = std::chrono::steady_clock::now();
    }

    void endExecution()
    {
        endTime = std::chrono::steady_clock::now();
    }
    void DisplayJobTiming(ofstream &outputFile)
    {
        const int columnWidth = 25;

        std::chrono::duration<double> elapsedSeconds = endTime - startTime;

        outputFile << left << setw(columnWidth) << "Job ID: " << setw(columnWidth) << JobID << endl;
        outputFile << left << setw(columnWidth) << "Start Time: " << setw(columnWidth) << startTime.time_since_epoch().count() << " ns" << endl;
        outputFile << left << setw(columnWidth) << "End Time: " << setw(columnWidth) << endTime.time_since_epoch().count() << " ns" << endl;
        outputFile << left << setw(columnWidth) << "Execution Time: " << setw(columnWidth) << elapsedSeconds.count() << " s" << endl;
    }
};

// Struct to represent an element with its priority
struct QueueElement
{
    Job *job;
    int priority;

    QueueElement(Job *j = nullptr, int pri = -1) : job(j), priority(pri) {}
};

class PriorityQueue
{
private:
    int maxSize;
    QueueElement *elements;
    int rearIndex;

public:
    PriorityQueue(int i = -1) : maxSize(i), rearIndex(-1)
    {
        if (maxSize <= 0)
        {
            cout << "Queue Size cannot be that low!\n ";
            exit(0);
        }
        else
            elements = new QueueElement[maxSize];
    }

    void heapifyUp()
    {
        int currentIndex = rearIndex;
        while (currentIndex > 0)
        {
            int parentIndex = (currentIndex - 1) / 2;
            if (elements[currentIndex].priority > elements[parentIndex].priority)
            {
                swap(elements[currentIndex], elements[parentIndex]);
                currentIndex = parentIndex;
            }
            else
            {
                break;
            }
        }
    }

    void enqueue(Job *job, int priority)
    {
        if (rearIndex == maxSize - 1)
        {
            throw out_of_range("PriorityQueue is full");
        }

        QueueElement newElement(job, priority);
        elements[++rearIndex] = newElement;

        heapifyUp();
    }

    bool isEmpty() const
    {
        return rearIndex == -1;
    }

    void displayQueue() const
    {
        if (isEmpty())
        {
            cout << "PriorityQueue is empty." << endl;
            return;
        }

        cout << "Priority Queue Contents:" << endl;
        for (int i = 0; i <= rearIndex; ++i)
        {
            cout << "Priority: " << elements[i].priority << " - Job ID: " << elements[i].job->JobID << endl;
        }
    }

    void heapifyDown()
    {
        int currentIndex = 0;
        while (true)
        {
            int leftChild = 2 * currentIndex + 1;
            int rightChild = 2 * currentIndex + 2;
            int largest = currentIndex;

            if (leftChild <= rearIndex && elements[leftChild].priority > elements[largest].priority)
            {
                largest = leftChild;
            }

            if (rightChild <= rearIndex && elements[rightChild].priority > elements[largest].priority)
            {
                largest = rightChild;
            }

            if (largest != currentIndex)
            {
                swap(elements[currentIndex], elements[largest]);
                currentIndex = largest;
            }
            else
            {
                break;
            }
        }
    }

    Job *dequeue()
    {
        if (isEmpty())
        {
            throw out_of_range("PriorityQueue is empty");
        }

        Job *frontJob = elements[0].job;

        // Replace the root with the last element
        elements[0] = elements[rearIndex--];

        // Rearrange elements to maintain priority order
        heapifyDown();

        return frontJob;
    }
};

class VM
{
public:
    const int TotalRam = 16;
    const int TotalProcessor = 16;
    const int TotalRTC = 16;

    int currentRam;
    int currentProcessor;
    int currentRTC;
    int MAX_CAPACITY;
    int currentJobNo;

public:
    Job **jobs;

    VM(int capacity = 10)
    {
        jobs = new Job *[capacity];
        currentRam = TotalRam;
        currentProcessor = TotalProcessor;
        currentRTC = TotalRTC;
        MAX_CAPACITY = capacity;
        currentJobNo = 0;
    }

    bool Send_Requestfor_NewVM()
    {
        // Add logic for sending a request for a new VM
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2); // green font
        cout << "Requesting for new VM...." << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // white font
        return false;
    }

    void showJob()
    {
        for (int i = 0; i < currentJobNo; i++)
        {
            cout << endl;
            cout << "ID: " << jobs[i]->JobID << endl;
            cout << "Priority: " << jobs[i]->Priority << endl;
        }
    }

    void showCurrentResource()
    {
        const int columnWidth = 20;

        cout << left << setw(columnWidth) << "VM has Ram       :" << currentRam << " GB" << endl;
        cout << left << setw(columnWidth) << "VM has Processor :" << currentProcessor << " GHz" << endl;
        cout << left << setw(columnWidth) << "VM has RTC       :" << currentRTC << " Hz" << endl;
    }

    void currentJob(Job *job)
    {
        cout << "Current Job details" << endl;
        job->DisplayJobDetails();
    }

    bool SendRequest()
    {
        return currentJobNo == MAX_CAPACITY;
    }

    bool PushJobs(Job *job)
    {
        if (currentJobNo == MAX_CAPACITY || currentRam <= TotalRam / 4 || currentProcessor <= TotalProcessor / 4 || currentRTC <= TotalRTC / 4)
        {
            cout << "Reached capacity" << endl;
            return false;
        }
        else
        {
            jobs[currentJobNo] = job;

            currentRam -= job->Ram;
            currentProcessor -= job->Processor;
            currentRTC -= job->RTC;

            cout << "=============================CURRENT VM RESOURCES===================================" << endl;
            cout << endl;
            showCurrentResource();
            currentJobNo++;
            return true;
        }
    }

    ~VM()
    {
        for (int i = 0; i < currentJobNo; i++)
        {
            delete jobs[i];
        }
        delete[] jobs;
    }
};

class CloudResourceManager
{
public:
    static int vmcounter;
    VM *currentVM;
    VM *DefaultVM;
    int MaxVMcapacity;

    CloudResourceManager(int size = 10) : MaxVMcapacity(size)
    {
        DefaultVM = new VM();
        currentVM = DefaultVM;
        vmcounter++;
    }

    VM *dropnewVM()
    {
        VM *newVM = new VM();
        vmcounter++;
        return newVM;
    }

    bool AddJobs(Job *job, VM *vm)
    {
        if (vm->PushJobs(job))
        {
            cout << endl;
            cout << "Added successfully in VM " << vmcounter << endl;
            return true;
        }
        else
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4); // red font
            cout << "Unable to add job. VM is at full capacity." << endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // white font
            bool returnvalue = vm->Send_Requestfor_NewVM();
            return returnvalue;
        }
    }
};

int CloudResourceManager::vmcounter = 0;

class Controller
{
public:
    int MAX_JOBS;
    Job *jobs;
    Job *DequeuedJob;
    CloudResourceManager manager;
    int job_No;
    int i;
    PriorityQueue *q1;
    ofstream logFile;
    int random;

    Controller()
    {
        READINGFILE();
        q1 = new PriorityQueue(MAX_JOBS);
        logFile.open("logfile.txt");
    }

    bool READINGFILE()
    {
        ifstream inputFile("jobs.txt");

        if (!inputFile.is_open())
        {
            cerr << "Error opening the file." << endl;
            return false;
        }

        // Count the number of lines in the file
        int count = 0;
        string line;
        while (getline(inputFile, line))
        {
            ++count;
        }

        inputFile.close();

        // Update MAX_JOBS based on the actual number of lines
        MAX_JOBS = count;

        // Allocate memory for jobs based on the number of lines
        jobs = new Job[MAX_JOBS];
        DequeuedJob = new Job[MAX_JOBS];

        // Read the file and populate the jobs array
        inputFile.open("jobs.txt");

        if (!inputFile.is_open())
        {
            cerr << "Error opening the file." << endl;
            return false;
        }

        count = 0;
        while (getline(inputFile, line) && count < MAX_JOBS)
        {
            istringstream iss(line);
            char comma;

            if (!(iss >> jobs[count].JobID >> comma >> jobs[count].Priority >> comma >>
                  jobs[count].Ram >> comma >> jobs[count].Processor >> comma >> jobs[count].RTC))
            {
                cerr << "Error reading line: " << count + 1 << endl;
                return false;
            }

            count++;
        }

        inputFile.close();

        return true;
    }

    void FaultTolerance(const Job &job)
    {
        // Implement fault tolerance logic here
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4); // red font
        cout << "Fault tolerance applied. Skipped job with JobID: " << job.JobID << endl;
        cout << "Because it requires too much computation " << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // red font
        // Additional fault tolerance actions if needed
    }

    void PRIORITIZE()
    {
        // **************Priority Queue

        srand(time(0));
        for (int i = 0; i < MAX_JOBS; i++) // Input 5 jobs at a time.
        {
            q1->enqueue(&jobs[i], jobs[i].Priority);
        }
        random = rand() % 30;
        for (int i = 0; i < MAX_JOBS; i++)
            if (!q1->isEmpty())
            {
                Job *frontJob = q1->dequeue(); // Get the Job from the queue
                job_No = frontJob->JobID;

                DequeuedJob[i] = *frontJob;
                if (DequeuedJob[i].JobID <= 0 || DequeuedJob[i].JobID >= 30 || DequeuedJob[i].JobID == random)
                {
                    applyFaultTolerance(DequeuedJob[i]);
                }
                else
                {
                    SENDJOB_TO_VM(i);
                }
                delete frontJob;
            }
    }

    void start()
    {
        PRIORITIZE();
    }

    void SENDJOB_TO_VM(int i)
    {
        if (DequeuedJob[i].JobID == 0)
        {
            // Job number is 0, apply fault tolerance logic
            FaultTolerance(DequeuedJob[i]);
            return;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1); // green font
        cout << "Sending jobNO : ";
        cout << DequeuedJob[i].JobID << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // white font

        // Record start time before sending to VM
        DequeuedJob[i].startExecution();

        // for loop sending automatically jobs
        if (manager.AddJobs(&DequeuedJob[i], manager.currentVM) == false)
        {
            VM *newVM = manager.dropnewVM();
            manager.currentVM = newVM;
        }

        // Record end time after sending to VM
        DequeuedJob[i].endExecution();

        // Write job timing details to the logfile
        logFile << "Job Timing Details:" << endl;
        logFile << "===================" << endl;
        DequeuedJob[i].DisplayJobTiming(logFile);
        logFile << "==============================================" << endl;
    }

    void applyFaultTolerance(const Job &job)
    {
        // Implement fault tolerance logic here
        // For example, you can log the fault, skip the job, or take other corrective actions
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4); // red font
        cout << "Fault tolerance applied. Skipped job with JobID: " << job.JobID << endl;
        cout << "Because it requires too much computation " << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // white font
        // Additional fault tolerance actions if needed
    }

    void setColor(int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void displayLogFileData()
    {
        ifstream logFile("logfile.txt");

        if (!logFile.is_open())
        {
            cerr << "Error opening the logfile." << endl;
            return;
        }

        cout << "Logfile Data:" << endl;
        cout << "===================" << endl;

        string line;
        while (getline(logFile, line))
        {
            // Check if the line contains specific markers and apply colors accordingly
            if (line.find("Job Timing Details:") != string::npos)
            {
                setColor(2); // Set text color to green for section headers
            }
            else if (line.find("Execution Time:") != string::npos)
            {
                setColor(5); // Set text color to purple for execution time
            }
            else if (line.find("===================") != string::npos)
            {
                setColor(7); // Set text color back to default for separators
            }
            else if (line.find("==============================================") != string::npos)
            {
                setColor(7); // Set text color back to default for separators
            }

            cout << line << endl;

            // Set text color back to default after printing the line
            setColor(7);
        }

        cout << "===================" << endl;

        logFile.close();
    }

    ~Controller()
    {
        logFile.close();
    }
};

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Function to display a menu with options
void displayMenu()
{
    cout << "==============================================" << endl;
    cout << "                 Cloud Manager                 " << endl;
    cout << "==============================================" << endl;
    cout << "1. Run Cloud Machines" << endl;
    cout << "2. View Logfile Data" << endl;
    cout << "3. Exit" << endl;
    cout << "==============================================" << endl;
    cout << "Enter your choice (1, 2, or 3): ";
}

int main()
{
    int choice;
    Controller c;
    do
    {
        displayMenu();
        cin >> choice;

        switch (choice)
        {
        case 1:
            setColor(2); // Set text color to green
            cout << "Running Cloud Machines..." << endl;
            setColor(7); // Set text color back to default
            c.start();   // Run Cloud Machines
            break;

        case 2:
            setColor(5); // Set text color to purple
            cout << "Viewing Logfile Data..." << endl;
            setColor(7);            // Set text color back to default
            c.displayLogFileData(); // View Logfile Data
            break;

        case 3:
            cout << "Exiting Cloud Manager. Goodbye!" << endl;
            break;

        default:
            setColor(4); // Set text color to red for invalid choice
            cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
            setColor(7); // Set text color back to default
            break;
        }

    } while (choice != 3);

    return 0;
}