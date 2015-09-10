void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

void expr_test();

int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the ``CPU_state'' structure. */
	reg_test();
	
	/* ZBY: test expr() */
	expr_test();

	/* Initialize the virtual computer system. */
	restart();

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
