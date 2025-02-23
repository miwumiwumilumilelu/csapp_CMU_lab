/*---------------------------------------------------------------------------*/
proxy.c:
/*---------------------------------------------------------------------------*/
 头文件与全局定义
/*---------------------------------------------------------------------------*/

#include "csapp.h"  // 引入CSAPP库，封装了Socket、I/O、错误处理等函数

// 定义缓存和对象的最大尺寸（代码中未实现缓存功能）
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

// 定义URI中的Web前缀（用于解析目标服务器地址）
#define WEB_PREFIX "http://"

// 固定的User-Agent头（标识代理服务器）
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 ... Firefox/10.0.3\r\n";

WEB_PREFIX 用于识别URI中的目标服务器地址（如 http://example.com）。
user_agent_hdr 是代理的固定标识，避免目标服务器拒绝请求。
/*---------------------------------------------------------------------------*/
辅助函数声明
/*---------------------------------------------------------------------------*/
void handleWrite();  // 处理EPIPE错误（客户端提前关闭连接）
void handleRequest(int);  // 处理客户端请求的核心函数
void clientError(int, char*, char*, char*, char*);  // 返回HTTP错误响应
int readAndFormatRequestHeader(rio_t*, char*, char*, char*, ...);  // 重构请求头
int checkGetMethod(char*, char*, char*);  // 检查是否为GET方法（未实现）
void replaceHTTPVersion(char*);  // 将HTTP/1.1改为1.0
void parseLine(char*, char*, char*, ...);  // 解析请求行
/*---------------------------------------------------------------------------*/
main 函数：启动代理服务器
/*---------------------------------------------------------------------------*/
int main(int argc, char** argv) {
    if (argc != 2) {  // 检查参数合法性
        unix_error("proxy usage: ./proxy <port number>");
    }

    int listenfd = Open_listenfd(argv[1]);  // 创建监听套接字
    struct sockaddr_storage clientAddr;
    char hostName[MAXLINE], port[MAXLINE];

    while(1) {  // 无限循环接受连接
        socklen_t addrLen = sizeof(clientAddr);
        int connfd = Accept(listenfd, (SA*)&clientAddr, &addrLen);  // 接受连接

        Getnameinfo((SA*)&clientAddr, addrLen, hostName, MAXLINE, port, MAXLINE, 0);
        printf("Accepting Connection from (%s, %s) \n", hostName, port);

        handleRequest(connfd);  // 处理请求
        Close(connfd);  // 关闭连接
    }
}

检查命令行参数（代理监听的端口）。
调用 Open_listenfd 创建监听套接字。
循环调用 Accept 接受客户端连接。
调用 handleRequest 处理请求，完成后关闭连接。
/*---------------------------------------------------------------------------*/
handleRequest 函数：处理客户端请求
/*---------------------------------------------------------------------------*/
void handleRequest(int fd) {
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char host[MAXLINE], port[MAXLINE], fileName[MAXLINE];
    rio_t rio;  // 客户端请求的I/O缓冲区

    // 读取请求行（如 "GET http://example.com/index.html HTTP/1.1"）
    Rio_readinitb(&rio, fd);
    if (Rio_readlineb(&rio, buf, MAXLINE) == 0) {
        printf("empty request\n");
        return;
    }

    replaceHTTPVersion(buf);  // 将HTTP/1.1改为1.0（简化处理）
    parseLine(buf, host, port, method, uri, version, fileName);  // 解析请求行

    // 检查是否为GET方法
    if (strcasecmp(method, "GET") != 0) {
        clientError(fd, method, "501", "Not Implemented", "Proxy不支持此方法");
        return;
    }

    // 重构请求头并发送到目标服务器
    char clientRequest[MAXLINE];
    int rv = readAndFormatRequestHeader(&rio, clientRequest, host, port, ...);
    if (rv == 0) return;  // 无效请求

    // 连接到目标服务器（如 example.com:80）
    char hostName[100];
    char* colon = strstr(host, ":");
    strncpy(hostName, host, colon - host);  // 提取主机名（如 "example.com"）
    int clientfd = Open_clientfd(hostName, port);  // 创建到目标服务器的套接字

    // 转发请求并接收响应
    Rio_writen(clientfd, clientRequest, strlen(clientRequest));  // 发送请求
    rio_t rioTiny;
    Rio_readinitb(&rioTiny, clientfd);
    char tinyResponse[MAXLINE];
    int n;
    while ((n = Rio_readnb(&rioTiny, tinyResponse, MAXLINE)) != 0) {
        Rio_writen(fd, tinyResponse, n);  // 将响应返回给客户端
    }
    Close(clientfd);  // 关闭目标服务器连接
}

读取请求行：获取方法、URI、协议版本。

协议降级：强制使用HTTP/1.0，避免处理复杂特性（如持久连接）。

解析URI：从URI中提取目标服务器的主机名、端口和路径。

重构请求头：添加必要字段（如Host），移除无关字段（如Proxy-Connection）。

转发请求：连接到目标服务器，发送重构后的请求。

返回响应：将目标服务器的响应逐字节返回客户端。
/*---------------------------------------------------------------------------*/
parseLine 函数：解析请求行
/*---------------------------------------------------------------------------*/
void parseLine(char* buf, char* host, char* port, ...) {
    // 示例请求行："GET http://localhost:8080/home.html HTTP/1.1"
    sscanf(buf, "%s %s %s", method, uri, version);  // 分割为三部分

    // 解析URI中的主机名、端口和路径
    char* hostp = strstr(uri, WEB_PREFIX) + strlen(WEB_PREFIX);  // 跳过"http://"
    char* slash = strstr(hostp, "/");  // 找到路径起始位置（如 "/home.html"）
    char* colon = strstr(hostp, ":");  // 查找端口分隔符（如 ":8080"）

    // 提取主机名（如 "localhost"）
    strncpy(host, hostp, slash - hostp);

    // 提取端口（如 "8080"），若未指定则默认为80
    if (colon) {
        strncpy(port, colon + 1, slash - colon - 1);
    } else {
        strcpy(port, "80");  // 默认HTTP端口
    }

    // 提取文件名（如 "/home.html"）
    strcpy(fileName, slash);
}

从URI中分离出主机名、端口和路径。

若URI未指定端口（如 http://example.com/），默认使用80端口。
/*---------------------------------------------------------------------------*/
readAndFormatRequestHeader 函数：重构请求头
/*---------------------------------------------------------------------------*/
int readAndFormatRequestHeader(rio_t* rio, char* clientRequest, ...) {
    // 1. 构造新的请求行（如 "GET /home.html HTTP/1.0"）
    sprintf(clientRequest, "GET %s HTTP/1.0\r\n", fileName);

    // 2. 读取原始请求头，过滤并添加必要字段
    char buf[MAXLINE];
    int HostInfo = 0, UserAgent = 0, Connection = 0;
    while (strcmp(buf, "\r\n") != 0) {  // 读到空行结束
        Rio_readlineb(rio, buf, MAXLINE);
        if (strstr(buf, "Host:")) HostInfo = 1;
        if (strstr(buf, "User-Agent:")) UserAgent = 1;
        if (strstr(buf, "Connection:")) Connection = 1;
        if (strstr(buf, "Proxy-Connection:")) continue;  // 移除Proxy-Connection
        strcat(clientRequest, buf);  // 添加其他字段
    }

    // 3. 补全缺失的Host头
    if (!HostInfo) {
        sprintf(buf, "Host: %s\r\n", host);
        strcat(clientRequest, buf);
    }

    // 4. 添加固定User-Agent和Connection头
    if (!UserAgent) strcat(clientRequest, user_agent_hdr);
    strcat(clientRequest, "Connection: close\r\n");
    strcat(clientRequest, "Proxy-Connection: close\r\n");

    // 5. 添加空行结束请求头
    strcat(clientRequest, "\r\n");
    return 1;
}

强制使用HTTP/1.0：简化代理实现。

移除无关字段：如 Proxy-Connection。

补全必要字段：

Host：目标服务器的主机名（必需字段）。

User-Agent：使用固定值。

Connection: close：禁用持久连接。
/*---------------------------------------------------------------------------*/
clientError 函数：返回错误响应
void clientError(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg) {
    char buf[MAXLINE];
    // 构造HTTP响应头
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    // 构造HTML错误页面
    sprintf(buf, "<html><title>Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    // ...（更多HTML内容）
}

返回格式化的HTTP错误页面（如501 Not Implemented）
/*---------------------------------------------------------------------------*/
代理核心功能：
/*---------------------------------------------------------------------------*/
接收客户端HTTP请求。

解析目标服务器地址（从URI中提取主机名和端口）。

重构请求头，移除不兼容字段，添加必要字段。

将请求转发到目标服务器。

将目标服务器的响应返回客户端。
/*---------------------------------------------------------------------------*/
关键设计：
/*---------------------------------------------------------------------------*/
协议降级：强制使用HTTP/1.0，避免处理复杂特性。

请求头过滤：移除Proxy-Connection，添加固定User-Agent。

错误处理：对无效请求返回标准错误页面。
/*---------------------------------------------------------------------------*/
局限性：
/*---------------------------------------------------------------------------*/
无缓存：未实现缓存功能（需扩展）。

单线程：一次处理一个请求，无法并发（可改进为多线程/线程池）。

仅支持GET：未实现POST等其他方法。
/*---------------------------------------------------------------------------*/
改进建议
/*---------------------------------------------------------------------------*/
实现缓存：

使用LRU缓存策略，缓存响应内容。

根据MAX_CACHE_SIZE和MAX_OBJECT_SIZE限制缓存大小。

支持并发：

使用多线程或线程池处理并发请求。

增强协议兼容性：

支持HTTP/1.1的持久连接（Connection: keep-alive）。

处理分块传输编码（Transfer-Encoding: chunked）。

安全性增强：

过滤恶意请求（如过长的URI）。

支持HTTPS（需处理SSL/TLS）。
/*---------------------------------------------------------------------------*/

