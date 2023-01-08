#include <iostream>
using namespace std;
#include <string.h>
#include "ip.h"


IP::IP(String law) {
	
	law = law.trim();
	//find type of field from the law
	field = find_field(law);
	//zero initiating. we'll find the accurate ip in set_value
	ip_law = "0.0.0.0";

}

String IP::find_field(String law) {

	String* src_out;
	size_t src_size;
	law.split("src-ip", &src_out, &src_size);
	if (src_out[0].trim().data[0] == '=') {
		delete[] src_out;
		return String("src");
	}
	else
	{	
		delete[] src_out;
		return String("dst");
	}
}



bool IP::set_value(String value) {

	
	/*now, we'll get the IP of the law (left to "/")*/
	String cpy_value = value;
	String* equ_out;
	size_t equ_size;

	cpy_value.split("=", &equ_out, &equ_size);
	if (equ_out == NULL) {
		return false;
	}
	String ip_and_range = equ_out[1].trim(); // our law is right to '='
	delete[] equ_out;
	
	size_t slash_size;
	String* slash_out;
	ip_and_range.split("/", &slash_out, &slash_size);
	if (slash_out == NUll) {
		return false;
	}
	ip_law = slash_out[0]; //	IP is left to the '/'
	delete[] slash_out;


	return true;
}


bool IP::match(String packet) {

	if (set_value(law)) {
		/*extract the IP from the packet according to field:*/
		//1. divide to fields
		String* out_fields;
		size_t size_fields;
		packet.split(",", &out_fields, &size_fields);

		//2. divide each field according to '='
		for (size_t i = 0; i < size_fields; i++) {

			out_fields[i] = out_fields[i].trim();
			String* tmp_out;
			size_t tmp_size;
			out_fields[i].split("=", &tmp_out, &tmp_size);

			//3.identify the corret ip according to field
			if (tmp_out[0].trim().equals(field)) {
				//4.then the second sub-field is the ip!
				tmp_out[1] = tmp_out[1].trim();
				String ip_extern = tmp_out[1];
				delete[] tmp_out;
				break;
			}
			delete[] tmp_out;
		}

		delete[] out_fields;
	}

	/*now, we'll check matching between IP of law and IP of the outer packet*/
	unsigned int mask = get_mask(law);
	unsigned int ip_ext_int = ip_to_int(ip_extern);
	unsigned int ip_law_int = ip_to_int(ip_law);

	return (ip_ext_int & mask) == (ip_law_int & mask);
}



unsigned int IP::ip_to_int(String ext_ip) {

	String* out;
	size_t size;
	ext_ip.split(".", out, size);
	unsigned int arr[size];
	for (size_t i = 0; i < size; i++) {
		arr[i] = out[i].to_integer();
	}

	unsigned int ip_int;
	ip_int = arr[0] << 24 | arr[1] << 16 | arr[2] << 8 | arr[3];
	delete[] out;

	return ip_int;
}


unsigned int IP::get_mask(String law) {

	//extarct the length of mask
	String cpy_law = law;
	size_t size;
	String* out_str;
	cpy_law.split("/", &out_str, &size);
	int m = out_str[1].to_integer();
	delete[] out_str;

	// create the mask
	unsigned int mask = 0;
	mask = -1;
	mask = mask << (32 - m);

	return mask;
}