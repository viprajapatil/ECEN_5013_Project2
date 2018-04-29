/*
@file - test_sensor.c
@brief - Unit test functions for testing various sensor tasks
@author - Nikhil Divekar & Vipraja Patil
*/
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"


void test_gas_init(void **state)
{
	int status = gas_value();	
	assert_int_not_equal(status, 0);
}


int main(int argc, char **argv)
{
  const struct CMUnitTest tests[] = 
  {	
	cmocka_unit_test(test_light_init),

  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
