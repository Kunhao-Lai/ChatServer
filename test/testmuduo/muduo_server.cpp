#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>
#include <functional>
#include <iostream>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1. 组合TcpServer对象
2. 创建EventLoop事件循环对象的指针
3. 明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4. 在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写事件的回调函数
5. 设置合适的服务器线程数量，muduo库会自己分配I/O线程和worker线程
*/

class ChatServer
{
public:
    // 初始化TcpServer
    ChatServer(EventLoop *loop,
               InetAddress &listenAddr)
        : _server(loop, listenAddr, "ChatServer"), _loop(loop)
    {
        // 通过绑定器设置回调函数
        _server.setConnectionCallback(bind(&ChatServer::onConnection,
                                           this, _1));
        _server.setMessageCallback(bind(&ChatServer::onMessage,
                                        this, _1, _2, _3));
        // 设置EventLoop的线程个数
        _server.setThreadNum(4);
    }
    // 启动ChatServer服务
    void start()
    {
        _server.start();
    }

private:
    // TcpServer绑定的回调函数，当有新连接或连接中断时调用
    void onConnection(const TcpConnectionPtr &con)
    {
        if (con->connected())
        {
            cout << con->peerAddress().toIpPort() << " -> " << con->localAddress().toIpPort() << "state:online" << endl;
        }
        else
        {
            cout << con->peerAddress().toIpPort() << " -> " << con->localAddress().toIpPort() << "state:offline" << endl;
        }
    }
    // TcpServer绑定的回调函数，当有新数据时调用
    void onMessage(const TcpConnectionPtr &con, //连接
                   Buffer *buf,                 //缓冲区
                   Timestamp time)              //接收到数据的时间信息
    {
        string buffer = buf->retrieveAllAsString();
        cout << "recv data:" << buffer << " time:" << time.toString() << endl;
        con->send(buffer);
    }

private:
    TcpServer _server;
    EventLoop *_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr);
    server.start();
    loop.loop();
    return 0;
} 