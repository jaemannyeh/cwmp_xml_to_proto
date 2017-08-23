#

.PHONY: all clean show

XML_FILES := $(wildcard ./*.xml)
PROTO_FILES := $(patsubst %.xml, %.proto, $(notdir $(XML_FILES)))
PB_CC_FILES := $(patsubst %.xml, %.pb.cc, $(notdir $(XML_FILES)))
GRPC_PB_CC_FILES := $(patsubst %.xml, %.grpc.pb.cc, $(notdir $(XML_FILES)))

all: cwmp_xml_to_proto cwmp_proto_to_grpc $(PROTO_FILES)

cwmp_proto_to_grpc: cwmp_proto_to_grpc.cc
	c++ $< -o $@ -std=c++11 `pkg-config --cflags --libs protobuf`

cwmp_xml_to_proto: cwmp_xml_to_proto.cc
	c++ $< -o $@ -std=c++11 `xml2-config --cflags` -lxml2

%.proto: %.xml
	./cwmp_xml_to_proto $< > $@.proto ## .proto.proto
	@echo protoc --cpp_out=. $@.proto
	./cwmp_proto_to_grpc $@.proto > $@
	-rm $@.proto ## .proto.proto
	
%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $<

%.grpc.pb.cc: %.proto
	protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

clean:
	-rm *.proto *.pb.cc *.pb.h *.o cwmp_xml_to_proto cwmp_proto_to_grpc
	
show:
	@echo XML_FILES = $(XML_FILES)
	@echo PROTO_FILES = $(PROTO_FILES)
	@echo PB_CC_FILES = $(PB_CC_FILES)
	@echo GRPC_PB_CC_FILES = $(GRPC_PB_CC_FILES)
	
test: test.cc tr-069-1-0-0-full.pb.cc
	c++ test.cc tr-069-1-0-0-full.pb.cc -o test `pkg-config --cflags --libs protobuf`
	