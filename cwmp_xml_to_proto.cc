//
// sudo apt-get update
// sudo apt-get install libprotobuf-dev
// sudo apt-get install protobuf-compiler
//
// Download all CWMP files from // https://www.broadband-forum.org
//
// c++ cwmp_xml_to_proto.cc -std=c++11 `xml2-config --cflags` -o cwmp_xml_to_proto -lxml2
//
// ./cwmp_xml_to_proto tr-069-1-0-0-full.xml > tr-069-1-0-0-full.proto
// protoc --cpp_out=. tr-069-1-0-0-full.proto
//
// jaemannyeh
//

#include <iostream>
#include <iomanip>      // std::setw
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#include <libxml/tree.h>

using namespace std;

static int get_indent_from_object_name(const string &object_name) {
  int found = 0;
  size_t position = object_name.find(".{i}.",0);
  while (position != string::npos) {
    found++;
    position = object_name.find(".{i}.",position+1); 
  }
  return (count(object_name.begin(),object_name.end(),'.') - found);
}

static string get_message_from_object_name(const string &object_name) {
  string message;
  
  string obj_name = object_name;
  string::reverse_iterator riter = obj_name.rbegin();
  size_t pos;
  if ( *(riter+4) == '.' && *(riter+3) == '{' ) {
    pos = obj_name.rfind('.',obj_name.rfind('.')-5);
    message = (pos == string::npos) ? obj_name : obj_name.substr(pos+1);
    message.erase(message.end()-5,message.end()); // [first,last) 
  } else if (*riter == '.') {
    pos = obj_name.rfind('.',obj_name.rfind('.')-1);
    message = (pos == string::npos) ? obj_name : obj_name.substr(pos+1);
    message.erase(message.end()-1);
  }
                
  return message;
}

static string get_field_from_parameter_name(const string &parameter_name) {
  string field;
  string::const_iterator citer = parameter_name.cbegin();
  field += ::tolower(*citer);
  while (++citer<parameter_name.cend()) {
    if (isupper(*citer) && islower(*(citer-1))) {
      field += '_';
      field += ::tolower(*citer);
    } else {
      field += ::tolower(*citer);
    }
  }
  //string field = parameter_name;
  //std::transform(field.begin(), field.end(), field.begin(), ::tolower);
  return field;
}

static string left_margin(int indent) {
  return string(2*(indent-1),' ');
}

static bool is_not_print(char c) 
{  
    return !isprint((unsigned)c);  
} 

static string shrink_description(const char *description_content) {
  string description = description_content;
  description.erase(description.begin(), description.begin()+11);      
  description.erase(description.begin()+40, description.end());
  description.erase(remove_if(description.begin(),description.end(),is_not_print),description.end());
  std::replace( description.begin(), description.end(), '\n', ' ' );
  std::replace( description.begin(), description.end(), '\r', ' ' );
  return description;
}

static string get_scalar_value_type(xmlNode *node) {
  static map<string,string> tr2pb = {
    {"int","int32"},
    {"long","int64"},
    {"unsignedInt","uint32"},
    {"unsignedLong","uint64"},
    {"boolean","bool"},
    {"base64","bytes"},
    {"hexBinary","bytes"},
    {"list","string"},    
    {"dataType","string"},
    {"dateTime","string"},
    {"string","string"}
  };
  
  if (0 && node->next && xmlStrcmp(node->next->name,(const xmlChar *)"syntax")) {
    cout << node->next->name << endl;
    cout << node->next->next->name << endl;
    cout << node->next->next->children->name << endl;
    cout << node->next->next->children->next->name << endl;
  }
        
  string scalar_value_type;
  //cout << node->next->next->children->next->name << endl;
  scalar_value_type = tr2pb[(const char *)node->next->next->children->next->name];
  if (scalar_value_type.empty()) {
    scalar_value_type = "undefined";
  }
  return scalar_value_type;
}

static int cwmp_xml_walk_tree(xmlNode *the_node)
{
  static xmlNode *the_root_node = NULL;
  if (the_root_node==NULL) {
      the_root_node = the_node;  
  }

  static string prev_object_name;
  static int tags[16] = { 1 };
  static int indent = 0;
  static vector<string> messages (16);
  
  auto closing_curly_brackets = [] (int prev_indent,int indent) { 
    int gap = prev_indent - indent;
    while (gap >= 0) {
      cout << left_margin(indent+gap) << "}" << endl;
      string message = get_message_from_object_name(messages[indent+gap]);
      string field = get_field_from_parameter_name(message);
      //string field = message;
      //std::transform(field.begin(), field.end(), field.begin(), ::tolower);
      
      cout << left_margin(indent+gap)
           << ((*(messages[indent+gap].end()-2)=='}') ? "repeated " : "")
           << message << " " << field << " = " << ++tags[indent+gap-1] << "; // "
           << messages[indent+gap] <<  endl;  
      gap--;
    }    
  }; // lambda expression
  
  for (xmlNode *node = the_node; node; node = node->next) {
    if (xmlStrcmp(node->name, (const xmlChar *)"description")==0) { 
      string description = shrink_description((const char *)xmlNodeGetContent(node));
      if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"parameter")==0) {
        string parameter_name = (const char *)xmlGetProp(node->parent,(const xmlChar *)"name");
        string object_name = (const char *)xmlGetProp(node->parent->parent,(const xmlChar *)"name");
        indent = get_indent_from_object_name(object_name);
        if (prev_object_name != object_name) {
          ;
        } else {
          tags[indent]++;
        }
        if (messages[indent].empty()) {
          cout << "//"; // If paramaeter_name has no object_name, comment the parameter out.  
        }
        cout << left_margin(indent+1) << std::left << setw(10) << get_scalar_value_type(node)
             << setw(40) << get_field_from_parameter_name(parameter_name)
             << " = " << setw(3) << tags[indent] << "; // " << " " << description << endl;
      } else if (node->parent && xmlStrcmp(node->parent->name,(const xmlChar *)"object")==0) {
        string object_name = (const char *)xmlGetProp(node->parent,(const xmlChar *)"name");
        indent = get_indent_from_object_name(object_name);
        string message = get_message_from_object_name(object_name); 
        if (prev_object_name != object_name) {
          int prev_indent = get_indent_from_object_name(prev_object_name);
          closing_curly_brackets(prev_indent,indent);
          cout << left_margin(indent) << "message " << message << " { // " << object_name << endl;
          messages[indent] = object_name;
          prev_object_name = object_name;
          tags[indent] = 0;
        } else {
          ;
        }
      }
    }
    indent = cwmp_xml_walk_tree(node->children); // The max depth of recursive call could be 9.
  }

  if (the_root_node == the_node) {
    closing_curly_brackets(indent,2);
    cout << "}" << endl;
  }
  
  return indent;
}

static int cwmp_xml_to_proto(char *cwmp_xml_file_name) {
  xmlDocPtr doc =  xmlParseFile(cwmp_xml_file_name);
  
  xmlNode *node = xmlDocGetRootElement(doc);;
  
  string package_name(cwmp_xml_file_name);
  package_name.erase(package_name.rfind(".xml"));
  package_name = package_name.substr(package_name.rfind('/')+1);
  std::replace( package_name.begin(), package_name.end(), '-', '_' );
  
  cout << "// Generated by " << __FILE__ << endl;
  cout << "// source: " << cwmp_xml_file_name << endl;
  cout << endl;
  cout << "syntax = \"proto3\";" << endl;;
  cout << endl;
  cout << "package " << package_name << ';' << endl;
  cout << endl;
  
  cwmp_xml_walk_tree(node); 

  xmlFreeDoc(doc); 

  return 0;    
}

int main(int argc,char *argv[]) {
  cwmp_xml_to_proto(argv[1]);
  return 0;
}