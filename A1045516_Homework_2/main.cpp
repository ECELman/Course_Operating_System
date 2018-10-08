#include <iostream>
#include <cmath>
#include <windows.h>
#include <thread>
#include <vector>
#include <ctime>
#include <semaphore.h>

#define BUFFER_SIZE 5 // �]�wbuffer�ү�Ȧs��item�Ƴ̤j��5

using namespace std;

// �إߩһݭn��semaphore�ܼ�
sem_t sem_mutex;
sem_t sem_full;
sem_t sem_empty;

vector<string>buffer; // �Ȧsproducer�Ҳ��ͥX�Ӫ�item

// �Nproducer�Ҳ��ͪ�item��Jbuffer
void insert_item(string item)
{
    // �Nproducer�Ҳ��ͪ�item��Jbuffer
    buffer.push_back(item);
}

// consumer�Nbuffer���ݪ�item�����X��
string remove_item()
{
    // ���X�s�bbuffer�̧��ݪ�item�A�åB�N�̧��ݪ�item������
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
        // �x�s�r��
        string item = "";

        // ��rand�X�@�ӼƦr���� 1 ~ 2^producer_id�A�p�Grand�X�Ӫ��ƭȤj��99�A�h�N��ƭȳ]��99
        int base = (int)(pow(2, producer_id));
        int rand_time = rand()%base + 1;
        if(rand_time > 99) rand_time = 99;

        // �̷ӭ��rand�X�Ӫ��Ʀrsleep���������
        Sleep(rand_time*1000);

        // producer���[�c
        sem_wait(&sem_empty);
        sem_wait(&sem_mutex);

        // ��rand�X�Ӫ��ƭȼȦs��item
        while(rand_time)
        {
            int temp = rand_time%10;
            item = (char)(temp + '0') + item;
            rand_time/=10;
        }

        // ���prand�X�Ӫ��Ʀr�O 1 ~ 9�A�h�bitem�e���ɤW0
        if(item.length() == 1) item = "0" + item;

        // ��ǦC���ƭȤ]�Ȧs��item
        int temp_sequence_number = sequence_number++;
        while(temp_sequence_number)
        {
            int temp = temp_sequence_number%10;
            item = (char)(temp + '0') + item;
            temp_sequence_number/=10;
        }

        // �ǦC���ƭȤ��� 1 ~ 9�A�h�e���b��00�A�p�G���� 10 ~ 99�A�h�e���b��0
        if(item.length() == 3) item = "00" + item;
        else if(item.length() == 4) item = "0" + item;

        // �Nproducer_id�]�Ȧs��item
        int temp_producer_id = producer_id;
        while(temp_producer_id)
        {
            int temp = temp_producer_id%10;
            item = (char)(temp + '0') + item;
            temp_producer_id/=10;
        }

        // producer�Nitem���Jbuffer
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
        // ��rand�X�@�ӼƦr���� 1 ~ 2^consumer_id�A�p�Grand�X�Ӫ��ƭȤj��99�A�h�N��ƭȳ]��99
        int base = (int)(pow(2.0, consumer_id));
        int rand_time = rand()%base + 1;
        if(rand_time > 99) rand_time = 99;

        // �̷ӭ��rand�X�Ӫ��Ʀrsleep���������
        Sleep(rand_time*1000);

        // consumer���[�c
        sem_wait(&sem_full);
        sem_wait(&sem_mutex);

        // consumer����buffer�̪�item
        string removed_item = remove_item();
        cout << "consumer " << consumer_id << " consumed " << removed_item <<endl;

        sem_post(&sem_mutex);
        sem_post(&sem_empty);
    }
}

int main(int argc,char *argv[])
{
    // �N��J�i�Ӫ��ƾڱq�r���ഫ�����
    int input[3]={0};
    for(int i=1; i<argc; i++)
        for(int j=0; j<strlen(argv[i]); j++)
            input[i-1] = input[i-1]*10 + (argv[i][j] - '0');

    // ��l��semaphore
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, BUFFER_SIZE);

    // �إ�thread���Ȧs�Ŷ�
    vector<thread>multi_task;

    // �إ�producer
    for(int i=1; i<=input[1]; i++) multi_task.push_back(thread(producer, i));

    // �إ�consumer
    for(int i=1; i<=input[2]; i++) multi_task.push_back(thread(consumer, i));

    // �]�wmain thread���a��n�Φh�[
    Sleep(input[0]*1000);

    for(int i=0; i<(input[1]+input[2]); i++) multi_task[i].detach();

    return 0;
}
