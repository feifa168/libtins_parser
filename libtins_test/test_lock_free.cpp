#include <boost/lockfree/queue.hpp>
#include <boost/bind.hpp>
#include <boost/threadpool.hpp>
#include <boost/thread/mutex.hpp>

using namespace boost;
using namespace boost::threadpool;
using namespace boost::lockfree;


typedef struct _MYSTRUCT_
{
    int nID;
    char GUID[64];
    _MYSTRUCT_()
    {
        memset(this, 0x0, sizeof(_MYSTRUCT_));
    }
}tagMyStruct;


//指定使用固定大小的队列 
//boost::lockfree::fixed_sized<true>
//使用动态大小的队列 
//boost::lockfree::fixed_sized<false>
boost::lockfree::queue<tagMyStruct, fixed_sized<false> > que(0);
int g_nID = 0;
void Thread_SetQueue()
{
    bool bRet = false;

    //插入数据

    tagMyStruct tagIn;
    strcpy(tagIn.GUID, "1234556");

    while (1)
    {
        tagIn.nID = g_nID++;
        bRet = que.push(tagIn);
        boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(1000));
    }
}

void Thread_SetQueue2()
{
    bool bRet = false;

    //插入数据

    tagMyStruct tagIn;
    strcpy(tagIn.GUID, "222222");

    while (1)
    {
        tagIn.nID = g_nID++;
        bRet = que.push(tagIn);
        boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(500));
    }
}

void Thread_GetQueue()
{
    bool bRet = false;

    //取出数据
    tagMyStruct tagOut;

    while (1)
    {
        if (que.pop(tagOut))
        {
            printf("取出一个数:ID=%d  GUDI=%s\n", tagOut.nID, tagOut.GUID);
        }
        else
            boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(200));;
    }
}

int test_lock_free()
{

    pool QueThread(3);

    QueThread.schedule(&Thread_SetQueue);

    QueThread.schedule(&Thread_SetQueue2);

    QueThread.schedule(&Thread_GetQueue);

    QueThread.wait();
    return 0;
}