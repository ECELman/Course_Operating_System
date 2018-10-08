#include <iostream>
#include <cmath>
#include <windows.h>
#include <thread>
#include <vector>
#include <ctime>
#include <semaphore.h>

#define BUFFER_SIZE 5 // 設定buffer所能暫存的item數最大為5

using namespace std;

// 建立所需要的semaphore變數
sem_t sem_mutex;
sem_t sem_full;
sem_t sem_empty;

vector<string>buffer; // 暫存producer所產生出來的item

// 將producer所產生的item放入buffer
void insert_item(string item)
{
    // 將producer所產生的item放入buffer
    buffer.push_back(item);
}

// consumer將buffer尾端的item給拿出來
string remove_item()
{
    // 拿出存在buffer最尾端的item，並且將最尾端的item給移除
    string temp_item = buffer.back();
    buffer.pop_back();

    return temp_item;
}

int sequence_number = 1;
void producer(int producer_id)
{
    srand(time(NULL));

    while(true)
    {
        // 儲存字串
        string item = "";

        // 先rand出一個數字介於 1 ~ 2^producer_id，如果rand出來的數值大於99，則將其數值設為99
        int base = (int)(pow(2, producer_id));
        int rand_time = rand()%base + 1;
        if(rand_time > 99) rand_time = 99;

        // 依照剛剛rand出來的數字sleep對應的秒數
        Sleep(rand_time*1000);

        // producer的架構
        sem_wait(&sem_empty);
        sem_wait(&sem_mutex);

        // 把rand出來的數值暫存到item
        while(rand_time)
        {
            int temp = rand_time%10;
            item = (char)(temp + '0') + item;
            rand_time/=10;
        }

        // 假如rand出來的數字是 1 ~ 9，則在item前面補上0
        if(item.length() == 1) item = "0" + item;

        // 把序列的數值也暫存到item
        int temp_sequence_number = sequence_number++;
        while(temp_sequence_number)
        {
            int temp = temp_sequence_number%10;
            item = (char)(temp + '0') + item;
            temp_sequence_number/=10;
        }

        // 序列的數值介於 1 ~ 9，則前面在補00，如果介於 10 ~ 99，則前面在補0
        if(item.length() == 3) item = "00" + item;
        else if(item.length() == 4) item = "0" + item;

        // 將producer_id也暫存到item
        int temp_producer_id = producer_id;
        while(temp_producer_id)
        {
            int temp = temp_producer_id%10;
            item = (char)(temp + '0') + item;
            temp_producer_id/=10;
        }

        // producer將item插入buffer
        insert_item(item);
        cout << "producer " << producer_id << " produced " << item << endl;

        sem_post(&sem_mutex);
        sem_post(&sem_full);
    }
}

void consumer(int consumer_id)
{
    while(true)
    {
        // 先rand出一個數字介於 1 ~ 2^consumer_id，如果rand出來的數值大於99，則將其數值設為99
        int base = (int)(pow(2.0, consumer_id));
        int rand_time = rand()%base + 1;
        if(rand_time > 99) rand_time = 99;

        // 依照剛剛rand出來的數字sleep對應的秒數
        Sleep(rand_time*1000);

        // consumer的架構
        sem_wait(&sem_full);
        sem_wait(&sem_mutex);

        // consumer移除buffer裡的item
        string removed_item = remove_item();
        cout << "consumer " << consumer_id << " consumed " << removed_item <<endl;

        sem_post(&sem_mutex);
        sem_post(&sem_empty);
    }
}

int main(int argc,char *argv[])
{
    // 將輸入進來的數據從字串轉換成整數
    int input[3]={0};
    for(int i=1; i<argc; i++)
        for(int j=0; j<strlen(argv[i]); j++)
            input[i-1] = input[i-1]*10 + (argv[i][j] - '0');

    // 初始化semaphore
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, BUFFER_SIZE);

    // 建立thread的暫存空間
    vector<thread>multi_task;

    // 建立producer
    for(int i=1; i<=input[1]; i++) multi_task.push_back(thread(producer, i));

    // 建立consumer
    for(int i=1; i<=input[2]; i++) multi_task.push_back(thread(consumer, i));

    // 設定main thread的地方要睡多久
    Sleep(input[0]*1000);

    for(int i=0; i<(input[1]+input[2]); i++) multi_task[i].detach();

    return 0;
}
