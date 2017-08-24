# cwmp_xml_to_proto

XML to Protocol Buffers converter for CWMP xml files from https://www.broadband-forum.org

This is for CWMP xml files from https://www.broadband-forum.org.

It coneverts the following files to proto files.

    tr-069-1-0-0-full.xml
    tr-098-1-8-0-full.xml
    tr-104-2-0-0-full.xml
    tr-106-1-2-0-full.xml
    tr-135-1-4-0-full.xml
    tr-140-1-3-0-full.xml
    tr-196-2-1-0-full.xml

Have a look at the files, and use the terminal to build them:

    $ make clean
    rm *.methods *.proto *.pb.cc *.pb.h *.o cwmp_xml_to_proto cwmp_proto_to_grpc
    rm: cannot remove ‘*.o’: No such file or directory
    make: [clean] Error 1 (ignored)

    $ make default
    c++ cwmp_xml_to_proto.cc -o cwmp_xml_to_proto -std=c++11 `xml2-config --cflags` -lxml2
    c++ cwmp_proto_to_grpc.cc -o cwmp_proto_to_grpc -std=c++11 `pkg-config --cflags --libs protobuf`

    $ make tr-069
    ./cwmp_xml_to_proto --package_name=tr069 tr-069-1-0-0-full.xml
    ./cwmp_proto_to_grpc --service_name=Gateway tr-069-1-0-0-full.proto > tr-069-1-0-0-full.1.proto
    mv tr-069-1-0-0-full.1.proto tr-069-1-0-0-full.proto 

    $ make pb_cc
    protoc -I ./ --cpp_out=. tr-069-1-0-0-full.proto
    protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` tr-069-1-0-0-full.proto

    $ make pb_obj
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-069-1-0-0-full.pb.o tr-069-1-0-0-full.pb.cc
    g++ -std=c++11 -I/usr/local/include -pthread  -c -o tr-069-1-0-0-full.grpc.pb.o tr-069-1-0-0-full.grpc.pb.cc
    
To test

    $ make test-tr-069
    c++ test-tr-069.cc tr-069-1-0-0-full.pb.cc -o test-tr-069 `pkg-config --cflags --libs protobuf`
    $ ./test-tr-069
    ......
    $ ./test-tr-069
    ......
    
jaemannyeh    

