#include "zb_common.h"
#include "zb_pooled_list.h"

typedef struct
{
  ZB_POOLED_LIST8_FIELD( test_data_link );
  int a;
  short int b;
} test8_t;

int list8_test( void )
{
	test8_t test[ 10 ];
	ZB_POOLED_LIST8_DEF( list );
	int index = 0;
	int i = 0;
	int result[ 4 ] = { 1, 2, 6, 3 };
	int result2[ 3 ] = { 1, 2, 3 };

	test[0].a = 0;
	test[0].b = 0;
	ZB_POOLED_LIST8_INSERT_TAIL( test, list, test_data_link, 0 );

	test[1].a = 1;
	test[1].b = 1;
	ZB_POOLED_LIST8_INSERT_TAIL( test, list, test_data_link, 1 );

	test[2].a = 2;
	test[2].b = 2;
	ZB_POOLED_LIST8_INSERT_TAIL( test, list, test_data_link, 2 );

	test[3].a = 3;
	test[3].b = 3;
	ZB_POOLED_LIST8_INSERT_TAIL( test, list, test_data_link, 3 );

	test[4].a = 4;
	test[4].b = 4;
	ZB_POOLED_LIST8_INSERT_TAIL( test, list, test_data_link, 4 );

	ZB_POOLED_LIST8_CUT_HEAD( test, list, test_data_link, index );

	test[5].a = 5;
	test[5].b = 5;
	ZB_POOLED_LIST8_INSERT_HEAD( test, list, test_data_link, 5 );

	test[6].a = 6;
	test[6].b = 6;
	ZB_POOLED_LIST8_INSERT_AFTER( test, list, test_data_link, 2, 6 );

	ZB_POOLED_LIST8_REMOVE_HEAD( test, list, test_data_link );
	ZB_POOLED_LIST8_REMOVE_TAIL( test, list, test_data_link );

	ZB_POOLED_LIST8_ITERATE( test, list, test_data_link, index )
	{
		if( result[ i ] != test[ index ].a )
		{
			return -1;
		}
		i++;
	}

	ZB_POOLED_LIST8_ITERATE_BACK( test, list, test_data_link, index )
	{
		i--;
		if( result[ i ] != test[ index ].a )
		{
			return -1;
		}
	}

	ZB_POOLED_LIST8_REMOVE( test, list, test_data_link, 6 );
	i = 0;
	ZB_POOLED_LIST8_ITERATE( test, list, test_data_link, index )
	{
		if( result2[ i ] != test[ index ].a )
		{
			return -1;
		}
		i++;
	}
  return 0;
}

typedef struct
{
  ZB_POOLED_LIST16_FIELD( test_data_link );
  int a;
  short int b;
} test16_t;

int list16_test( void )
{
	test16_t test[ 10 ];
	ZB_POOLED_LIST16_DEF( list );
	int index = 0;
	int i = 0;
	int result[ 4 ] = { 1, 2, 6, 3 };
	int result2[ 3 ] = { 1, 2, 3 };

	test[0].a = 0;
	test[0].b = 0;
	ZB_POOLED_LIST16_INSERT_TAIL( test, list, test_data_link, 0 );

	test[1].a = 1;
	test[1].b = 1;
	ZB_POOLED_LIST16_INSERT_TAIL( test, list, test_data_link, 1 );

	test[2].a = 2;
	test[2].b = 2;
	ZB_POOLED_LIST16_INSERT_TAIL( test, list, test_data_link, 2 );

	test[3].a = 3;
	test[3].b = 3;
	ZB_POOLED_LIST16_INSERT_TAIL( test, list, test_data_link, 3 );

	test[4].a = 4;
	test[4].b = 4;
	ZB_POOLED_LIST16_INSERT_TAIL( test, list, test_data_link, 4 );

	ZB_POOLED_LIST16_CUT_HEAD( test, list, test_data_link, index );

	test[5].a = 5;
	test[5].b = 5;
	ZB_POOLED_LIST16_INSERT_HEAD( test, list, test_data_link, 5 );

	test[6].a = 6;
	test[6].b = 6;
	ZB_POOLED_LIST16_INSERT_AFTER( test, list, test_data_link, 2, 6 );

	ZB_POOLED_LIST16_REMOVE_HEAD( test, list, test_data_link );
	ZB_POOLED_LIST16_REMOVE_TAIL( test, list, test_data_link );

	ZB_POOLED_LIST16_ITERATE( test, list, test_data_link, index )
	{
		if( result[ i ] != test[ index ].a )
		{
			return -1;
		}
		i++;
	}

	ZB_POOLED_LIST16_ITERATE_BACK( test, list, test_data_link, index )
	{
		i--;
		if( result[ i ] != test[ index ].a )
		{
			return -1;
		}
	}

	ZB_POOLED_LIST16_REMOVE( test, list, test_data_link, 6 );
	i = 0;
	ZB_POOLED_LIST16_ITERATE( test, list, test_data_link, index )
	{
		if( result2[ i ] != test[ index ].a )
		{
			return -1;
		}
		i++;
	}
  return 0;
}

MAIN()
{
  ARGV_UNUSED;

  TRACE_INIT("pooled_list_test");
  TRACE_MSG( TRACE_INFO1, "started", (FMT__0));

  TRACE_MSG( TRACE_INFO1, "started", (FMT__0));
  if( list8_test() == 0 )
  {
    TRACE_MSG( TRACE_INFO1, "Test8 OK\n", (FMT__0 ));
  }
  else
  {
    TRACE_MSG( TRACE_INFO1, "Test8 failed\n ", (FMT__0 ));
  }

  if( list16_test() == 0 )
  {
    TRACE_MSG( TRACE_INFO1, "Test16 OK\n", (FMT__0 ));
  }
  else
  {
    TRACE_MSG( TRACE_INFO1, "Test16 failed\n", (FMT__0 ));
  }

  MAIN_RETURN(0);
}
