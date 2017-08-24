//
// c++ test.cc tr-069-1-0-0-full.pb.cc -o test `pkg-config --cflags --libs protobuf`
// ./test
// hexdump -C device.dat 
//

#include <iostream>
#include <iomanip>      // std::setw
#include <fstream>
#include <string>

#include "tr-069-1-0-0-full.pb.h"

using namespace std;

static void test_1(tr069::InternetGatewayDevice &device) {
  cout << setw(40) << "landevice_number_of_entries " << device.landevice_number_of_entries() << endl;
  cout << setw(40) << "wandevice_number_of_entries " << device.wandevice_number_of_entries() << endl;

  device.set_landevice_number_of_entries(__LINE__);
  device.set_wandevice_number_of_entries(__LINE__);
}

static void test_2(tr069::InternetGatewayDevice &device) {
  cout << setw(40) << "uptime " << device.device_info().up_time() << endl;
  for (int i = 0; i < device.device_info().vendor_config_file_size(); i++) {
    //const tr069::InternetGatewayDevice_DeviceInfo_VendorConfigFile &vendorconfigfile = device.deviceinfo().vendorconfigfile(i);
    cout << setw(40) << "date " << device.device_info().vendor_config_file(i).date() << endl;
  }
  
  device.mutable_device_info()->set_up_time(device.device_info().up_time()+1);
  if (device.device_info().vendor_config_file_size()==0) {
    //tr069::InternetGatewayDevice_DeviceInfo_VendorConfigFile *vendorconfigfile;
    device.mutable_device_info()->add_vendor_config_file()->set_date(__DATE__);
  }
}

static void test_3(tr069::InternetGatewayDevice &device) {
  cout << setw(40) << "forwardnumberofentries " << device.layer3forwarding().forward_number_of_entries() << endl;
  for (int i = 0; i < device.layer3forwarding().forwarding_size(); i++) {
    cout << setw(40) << "mtu " << device.layer3forwarding().forwarding(i).mtu() << endl;
  }
  
  if (device.layer3forwarding().forwarding_size()<=4) {
    device.mutable_layer3forwarding()->add_forwarding()->set_mtu(__LINE__);
    device.mutable_layer3forwarding()->set_forward_number_of_entries(device.layer3forwarding().forwarding_size());
  }
}

static void test_4(tr069::InternetGatewayDevice &device) {
  if (device.wandevice_size()==0) {
    device.add_wandevice()->add_wanconnection_device()->add_wanpppconnection()->mutable_stats()->set_ethernet_packets_received(__LINE__);
  } else {
    int n = device.wandevice(0).wanconnection_device(0).wanpppconnection(0).stats().ethernet_packets_received()+1;
    device.mutable_wandevice(0)->mutable_wanconnection_device(0)->mutable_wanpppconnection(0)->mutable_stats()->set_ethernet_packets_received(n);
  }
  cout << setw(40) << "ethernetpacketsreceived " << device.wandevice(0).wanconnection_device(0).wanpppconnection(0).stats().ethernet_packets_received() << endl;
}

static int test() {
  string data_file_name(__FILE__);
  data_file_name.erase(data_file_name.rfind(".cc"));  
  data_file_name += ".dat";
  cout << data_file_name.c_str() << endl;
  
  tr069::InternetGatewayDevice device;

  fstream input(data_file_name.c_str(), ios::in | ios::binary); // % hexdump -C device.dat 
  if (!input) {
    cout << data_file_name << ": File not found.  Creating a new file." << endl;
  } else if (!device.ParseFromIstream(&input)) {
    cerr << "Failed to parse device data." << endl;
    return -1;
  }

  test_1(device);
  test_2(device);
  test_3(device);
  test_4(device);

  fstream output(data_file_name.c_str(), ios::out | ios::trunc | ios::binary);
  if (!device.SerializeToOstream(&output)) {
    cerr << "Failed to write device data." << endl;
    return -1;
  }

  return 0;
}

int main(int argc, char* argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  test();
  return 0;
}

