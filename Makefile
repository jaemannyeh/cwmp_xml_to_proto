#

CXXFLAGS += -std=c++11
CPPFLAGS += -I/usr/local/include -pthread

.PHONY: default proto all clean show

XML_FILES := $(wildcard ./*.xml)
PROTO_FILES := $(patsubst %.xml, %.proto, $(notdir $(XML_FILES)))
PB_CC_FILES := $(patsubst %.xml, %.pb.cc, $(notdir $(XML_FILES)))
GRPC_PB_CC_FILES := $(patsubst %.xml, %.grpc.pb.cc, $(notdir $(XML_FILES)))
PB_O_FILES := $(patsubst %.xml, %.pb.o, $(notdir $(XML_FILES)))
GRPC_PB_O_FILES := $(patsubst %.xml, %.grpc.pb.o, $(notdir $(XML_FILES)))

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
	protoc -I ./ --cpp_out=. $@.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $@.proto

tr-098: tr-098-1-8-0-full 	
tr-098-1-8-0-full:
	./cwmp_xml_to_proto --package_name=tr098 $@.xml
	./cwmp_proto_to_grpc --service_name=Gateway $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 
	
tr-196: tr-196-2-1-0-full
tr-196-2-1-0-full:
	./cwmp_xml_to_proto --package_name=tr196 $@.xml
	./cwmp_proto_to_grpc --service_name=FAP $@.proto > $@.1.proto
	mv $@.1.proto $@.proto 
	
proto: $(PROTO_FILES)

pb_cc: $(PB_CC_FILES) $(GRPC_PB_CC_FILES)

pb_obj: $(GRPC_PB_O_FILES) $(PB_O_FILES)

%.proto: %.xml
	./cwmp_xml_to_proto $<
	./cwmp_proto_to_grpc $@ > $@.1.proto
	-rm $@
	-mv $@.1.proto $@
	
%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $<

%.grpc.pb.cc: %.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

clean:
	-rm *.methods *.proto *.pb.cc *.pb.h *.o cwmp_xml_to_proto cwmp_proto_to_grpc
	
show:
	@echo XML_FILES = $(XML_FILES)
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo PB_CC_FILES = $(PB_CC_FILES)
	@echo GRPC_PB_CC_FILES = $(GRPC_PB_CC_FILES)
	@echo PB_O_FILES = $(PB_O_FILES)
	
test: test.cc tr-069-1-0-0-full.pb.cc
	c++ test.cc tr-069-1-0-0-full.pb.cc -o test `pkg-config --cflags --libs protobuf`
	