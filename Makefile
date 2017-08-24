#

CXXFLAGS += -std=c++11
CPPFLAGS += -I/usr/local/include -pthread

.PHONY: default clean show

XML_FILES := $(wildcard ./*.xml)
PROTO_FILES := $(wildcard ./*.proto) ## PROTO_FILES := $(patsubst %.xml, %.proto, $(notdir $(XML_FILES)))
PB_CC_FILES := $(patsubst %.proto, %.pb.cc, $(notdir $(PROTO_FILES)))
GRPC_PB_CC_FILES := $(patsubst %.proto, %.grpc.pb.cc, $(notdir $(PROTO_FILES)))
PB_O_FILES := $(patsubst %.proto, %.pb.o, $(notdir $(PROTO_FILES)))
GRPC_PB_O_FILES := $(patsubst %.proto, %.grpc.pb.o, $(notdir $(PROTO_FILES)))

default: cwmp_xml_to_proto cwmp_proto_to_grpc

cwmp_proto_to_grpc: cwmp_proto_to_grpc.cc
	c++ $< -o $@ -std=c++11 `pkg-config --cflags --libs protobuf`

cwmp_xml_to_proto: cwmp_xml_to_proto.cc
	c++ $< -o $@ -std=c++11 `xml2-config --cflags` -lxml2

tr-069: tr-069-1-0-0-full 	
tr-069-1-0-0-full:
	./cwmp_xml_to_proto --package_name=tr069 $@.xml
	./cwmp_proto_to_grpc --service_name=Gateway $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 

tr-098: tr-098-1-8-0-full 	
tr-098-1-8-0-full:
	./cwmp_xml_to_proto --package_name=tr098 $@.xml
	./cwmp_proto_to_grpc --service_name=Gateway $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 

tr-104: tr-104-2-0-0-full 	
tr-104-2-0-0-full:
	./cwmp_xml_to_proto --package_name=tr104 $@.xml
	./cwmp_proto_to_grpc --service_name=Vois $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 

tr-106: tr-106-1-2-0-full	
tr-106-1-2-0-full:
	./cwmp_xml_to_proto --package_name=tr106 $@.xml
	./cwmp_proto_to_grpc --service_name=Board $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 

tr-135: tr-135-1-4-0-full	
tr-135-1-4-0-full:
	./cwmp_xml_to_proto --package_name=tr135 $@.xml
	./cwmp_proto_to_grpc --service_name=Box $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 

tr-140: tr-140-1-3-0-full	
tr-140-1-3-0-full:
	./cwmp_xml_to_proto --package_name=tr140 $@.xml
	./cwmp_proto_to_grpc --service_name=Storage $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 
	
tr-196: tr-196-2-1-0-full
tr-196-2-1-0-full:
	./cwmp_xml_to_proto --package_name=tr196 $@.xml
	./cwmp_proto_to_grpc --service_name=FAP $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 
	
proto: $(PROTO_FILES)

pb_cc: $(PB_CC_FILES) $(GRPC_PB_CC_FILES)

pb_obj: $(PB_O_FILES) $(GRPC_PB_O_FILES)

%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $<

%.grpc.pb.cc: %.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

clean:
	-rm *.dat *.methods *.proto *.pb.cc *.pb.h *.o test-tr-069 cwmp_xml_to_proto cwmp_proto_to_grpc
	
show:
	@echo XML_FILES = $(XML_FILES)
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo PB_CC_FILES = $(PB_CC_FILES)
	@echo GRPC_PB_CC_FILES = $(GRPC_PB_CC_FILES)
	@echo PB_O_FILES = $(PB_O_FILES)
	@echo GRPC_PB_O_FILES = $(GRPC_PB_O_FILES)	
	
test-tr-069: test-tr-069.cc tr-069-1-0-0-full.pb.cc
	c++ test-tr-069.cc tr-069-1-0-0-full.pb.cc -o $@ `pkg-config --cflags --libs protobuf`
	