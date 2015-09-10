#include "cpu/exec/helper.h"

make_helper(exec);

make_helper(data_size) {
	ops_decoded.is_data_size_16 = true;
	int instr_len = exec(eip + 1);
	ops_decoded.is_data_size_16 = false;
	return instr_len + 1;
}

make_helper(addr_size) {
    // address size override : http://www.mouseos.com/x64/doc3.html
	ops_decoded.is_addr_size_16 = true;
	int instr_len = exec(eip + 1);
	ops_decoded.is_addr_size_16 = false;
	return instr_len + 1;
}
