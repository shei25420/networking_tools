cmake_minimum_required(VERSION 3.26)
project(scraper C)

set(CMAKE_C_STANDARD 11)
link_libraries(crypto)
link_libraries(ssl)
find_package(LibXml2 REQUIRED)
add_executable(scraper main.c
        DataStructures/LIsts/LinkedLists.c
        DataStructures/LIsts/LinkedLists.h
        DataStructures/Common/Node.c
        DataStructures/Common/Node.h
        DataStructures/Queues/Queue.c
        DataStructures/Queues/Queue.h
        DataStructures/Trees/BinaryTree.c
        DataStructures/Trees/BinaryTree.h
        System/ThreadPool.c
        System/ThreadPool.h
        Protocols/HttpClient.c
        Protocols/HttpClient.h
        Protocols/HTTP/HTTPRequest.c
        Protocols/HTTP/HTTPRequest.h
        Protocols/HTTP/HTTPResponse.c
        Protocols/HTTP/HTTPResponse.h
        WebScraper/HTMLLinksParser.c
        WebScraper/HTMLLinksParser.h
        WebScraper/WebScraper.c
        WebScraper/WebScraper.h
        WebScraper/BinaryNodeStack.c
        WebScraper/BinaryNodeStack.h
        DataStructures/Stack/Stack.c
        DataStructures/Stack/Stack.h
        IPLookup/IPLookup.c
        IPLookup/IPLookup.h
        DNSQuery/DNSQuery.c
        DNSQuery/DNSQuery.h
        Protocols/TCP/TCPServer.c
        Protocols/TCP/TCPServer.h
        LikeRedis/LIkeRedis.c
        LikeRedis/LIkeRedis.h
        Protocols/TCP/TCPClient.c
        Protocols/TCP/TCPClient.h
)
target_link_libraries(scraper PRIVATE ${LIBXML2_LIBRARIES})