#

.PHONY: all clean show

XML_FILES := $(wildcard ./*.xml)
PROTO_FILES := $(patsubst %.xml, %.proto, $(notdir $(XML_FILES)))

all: cwmp_xml_to_proto $(PROTO_FILES) test

cwmp_xml_to_proto: cwmp_xml_to_proto.cc
	c++ $< -o $@ -std=c++11 `xml2-config --cflags` -lxml2

test: test.cc tr-069-1-0-0-full.pb.cc
	c++ test.cc tr-069-1-0-0-full.pb.cc -o test `pkg-config --cflags --libs protobuf`
	
%.proto: %.xml
	./cwmp_xml_to_proto $< > $@
	protoc --cpp_out=. $@
	
clean:
	-rm *.proto *.pb.cc *.pb.h cwmp_xml_to_proto test device.dat
	
show:
	@echo XML_FILES = $(XML_FILES)
	@echo PROTO_FILES = $(PROTO_FILES)