//
// c++ cwmp_proto_to_grpc.cc -o cwmp_proto_to_grpc -std=c++11 `pkg-config --cflags --libs protobuf`
// ./cwmp_proto_to_grpc tr-098-1-8-0-full.proto > tr-098.proto
// protoc --cpp_out=. tr-098.proto
// jaemannyeh
//

#include <iostream>
#include <iomanip>      // std::setw
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#include <google/protobuf/compiler/importer.h>

using namespace std;

static string the_service_name("Board");
  
static int cwmp_proto_walk_tree(const google::protobuf::Descriptor* the_desc) {

  static string desc_names;
  
  for (int i=0; i<the_desc->field_count(); i++) {
    
    const google::protobuf::FieldDescriptor *field = the_desc->field(i);
    const google::protobuf::Descriptor* desc= field->message_type(); // If type is TYPE_MESSAGE or TYPE_GROUP, ....

    if (desc) { // if (field->cpp_type() == 10)
      std::string message;
      if (field->is_repeated()) {
        message = "stream " + desc->full_name();
      } else {
        message = desc->full_name();
      }
      
      string slash2;
      if (desc_names.find("Get"+desc->name()+" ") == string::npos) {
        desc_names += "Get"+desc->name()+" ";        
        slash2 = "  ";
      } else {
        slash2 = "  // ";
      }
      
      cout << slash2 << "rpc Get" << desc->name() << "(" << the_service_name << "Request" << ")" << " returns (" << message << ") {}" << endl;
      cout << slash2 << "rpc Set" << desc->name() << "(" << message << ")" << " returns (" << the_service_name << "Reply" << ") {}" << endl;
      cout << endl;
      
      cwmp_proto_walk_tree(desc);
    } 

  }

  return 0;  
}

static int cwmp_proto_to_grpc(const char *cwmp_proto_file_name) {
  
  google::protobuf::compiler::DiskSourceTree source_tree;
  source_tree.MapPath("", ".");
	google::protobuf::compiler::Importer importer(&source_tree,NULL);
	const google::protobuf::FileDescriptor* fd = importer.Import(cwmp_proto_file_name); // auto fd = importer.Import(cwmp_proto_file_name);
	assert(fd);	
	assert(fd->service_count()==0);
	assert(fd->message_type_count()==1); // Number of top-level message types defined in this file.

  cout << "// Generated by " << __FILE__ << endl;
  cout << "// source: " << fd->name() << endl;
  cout << "// 1st top-level message: " << fd->message_type(0)->name() << endl; // 1st top-level message types defined in this file.
  cout << endl;
  
  cout << "syntax = \"proto3\";" << endl << endl;
  
  cout << "import " << "\"" << fd->name() << "\";" << endl;
  cout << endl;
  
  cout << "message " << the_service_name << "Request { int32 unused = 1; }" << endl << endl;
  cout << "message " << the_service_name << "Reply { int32 unused = 1; }" << endl << endl;
  cout << "message " << "X_012345_Message { int32 unused = 1;  } // Vendor extensions X_{OUI}_{NameOfNewThing}" << endl << endl;
    
  cout << "service " << the_service_name << " { // " << endl;
  cwmp_proto_walk_tree(fd->message_type(0)); // 1st in top-level messages types defined in this file.
  cout << "}" << endl;
  
  return 0;    
}

int main(int argc,char *argv[]) {
	assert(argc==2);	  
  cwmp_proto_to_grpc(argv[1]);
  return 0;
}