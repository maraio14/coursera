__author__ = 'justme'
import sys
import csv
import datetime as dt
import pandas as pd
import numpy as np
import QSTK.qstkutil.qsdateutil as du
import QSTK.qstkutil.DataAccess as da
import QSTK.qstkutil.tsutil as tsu
import math


def marketSimulator(init_value, inFileName, outFileName):
	dt_timeofday = dt.timedelta(hours=16)
	reader = csv.reader(open(inFileName, 'rU'), delimiter=',')
	ls_order_dates = []
	ls_orders = []
	ls_columns = ['Order', 'Type', 'Quantity']
	for row in reader:
		dt_order = dt.datetime(int(row[0]), int(row[1]), int(row[2]))
		ls_order_dates.append(dt_order + dt_timeofday)
		ls_orders.append([row[3], row[4], row[5]])


	# reader.close()

	na_orders = np.array(ls_orders)

	orders_frm = pd.DataFrame(data=na_orders, index=ls_order_dates, columns=ls_columns)

	ls_equities = list(set(na_orders[:, 0]))
	set_order_dates = set(ls_order_dates)
	ls_dates_asc = sorted(list(set_order_dates))

	ldt_timestamps = du.getNYSEdays(ls_dates_asc[0], ls_dates_asc[len(ls_dates_asc) - 1], dt_timeofday)
	cash_available = int(init_value)
	ts_portfolio = pd.TimeSeries(0, index=ldt_timestamps)

	c_dataobj = da.DataAccess('Yahoo')
	#ls_keys = ['open', 'high', 'low', 'close', 'volume', 'actual_close']
	ls_keys = ['close']
	spx_symbol = '$SPX'
	ls_equities.append(spx_symbol)
	ldf_data = c_dataobj.get_data(ldt_timestamps, ls_equities, ls_keys)
	d_data = dict(zip(ls_keys, ldf_data))

	for s_key in ls_keys:
		d_data[s_key] = d_data[s_key].fillna(method='ffill')
		d_data[s_key] = d_data[s_key].fillna(method='bfill')

	na_price = d_data['close'].values

	cumm_daily_rets = tsu.returnize1(na_price.copy())

	#cumm_daily_price= cumm_daily_rets.copy()
	all_cols = ls_equities[:]
	all_cols.append('Value')

	na_zeros = np.zeros((len(ldt_timestamps), len(all_cols)))
	df_portval = pd.DataFrame(na_zeros, index=ldt_timestamps, columns=all_cols)

	writer = csv.writer(open(outFileName, 'w'), delimiter=',')

	df_portval.ix[ldt_timestamps[0]]['Value'] = cash_available
	port_value = 0

	for idx in xrange(0, len(cumm_daily_rets)):
		ldt_day = ldt_timestamps[idx]

		if idx > 0:
			df_portval.ix[ldt_day][:-1] = df_portval.ix[ldt_timestamps[idx - 1]][:-1] * cumm_daily_rets[idx]

		curr_prices = d_data['close'].ix[ldt_day]

		if ldt_day in set_order_dates:
			orders = orders_frm.ix[ldt_day].values
			orders = orders.reshape((orders.size // 3, 3))

			for order in orders:
				symbol = order[0]
				type = order[1]
				amount = int(order[2])

				price_symbol = curr_prices[symbol]

				total_order_value = amount * price_symbol
				if type == 'Buy':
					df_portval.ix[ldt_day][symbol] += total_order_value
					cash_available -= total_order_value
				else:
					df_portval.ix[ldt_day][symbol] -= total_order_value
					cash_available += total_order_value
		port_value = cash_available + np.sum(df_portval.ix[ldt_day][:-1], axis=0)

		if ldt_day in set_order_dates:
			writer.writerow((ldt_day.year, ldt_day.month, ldt_day.day, port_value, ''))

		df_portval.ix[ldt_day]['Value'] = port_value

	#print (df_portval)

	fund_daily_rets = tsu.returnize0(df_portval[:]['Value'].copy())
	fund_average_daily_rets = np.average(fund_daily_rets, axis=0)
	fund_STD_daily_rets = np.std(fund_daily_rets.copy(), axis=0)
	fund_sharp_ratio_daily = math.sqrt(252) * (fund_average_daily_rets / fund_STD_daily_rets)
	fund_cummulative_daily_rets = (df_portval.ix[ldt_timestamps[len(ldt_timestamps)-1]]['Value'] / df_portval.ix[ldt_timestamps[0]]['Value'] )

	SPX_daily_rets = tsu.returnize0(d_data['close'][spx_symbol].copy())
	SPX_average_daily_rets = np.average(SPX_daily_rets, axis=0)
	SPX_STD_daily_rets = np.std(SPX_daily_rets.copy(), axis=0)
	SPX_sharp_ratio_daily = math.sqrt(252) * (SPX_average_daily_rets / SPX_STD_daily_rets)
	SPX_cummulative_daily_rets = (d_data['close'][spx_symbol][-1] / d_data['close'][spx_symbol][0])


	dt_quizz = ldt_timestamps[len(ldt_timestamps) - 1]  #dt.datetime(2011, 11, 9)
	#dt_quizz = dt.datetime(2011, 9, 26)
	dt_quizz_shift = (dt_quizz + dt_timeofday)
	#print(df_portval.ix[dt_quizz_shift]['Value'])


	##compute the portfolio cumm_daily rets, not the original shares
	# daily_rets = cumm_daily_rets - 1
	# average_daily_rets = np.average(daily_rets, axis = 0)
	# STD_daily_rets = np.std(daily_rets.copy(), axis = 0)
	# sharp_ratio_daily = math.sqrt(252)* (average_daily_rets/STD_daily_rets)
	# cummulative_daily_rets = (df_portval.ix[ldt_timestamps[len(ldt_timestamps)-1]]['Value'] / df_portval.ix[ldt_timestamps[0]]['Value'] )

	print ('Details of the Performance of the portfolio :')
	print('')
	print('Data Range : ' + str(ldt_timestamps[0]) + ' to ' + str(ldt_timestamps[-1]) )
	print('')
	print('Sharpe Ratio of Fund : ' + str(fund_sharp_ratio_daily))
	print('Sharpe Ratio of $SPX : ' + str(SPX_sharp_ratio_daily))
	print('')
	print('Total Return of Fund : ' + str(fund_cummulative_daily_rets))
	print('Total Return of $SPX : ' + str(SPX_cummulative_daily_rets))
	print('')
	print('Standard Deviation of Fund : ' + str(fund_STD_daily_rets))
	print('Standard Deviation of $SPX : ' + str(SPX_STD_daily_rets))
	print('')
	print('Average Daily Return of Fund : ' + str(fund_average_daily_rets))
	print('Average Daily Return of $SPX : ' + str(SPX_average_daily_rets))

	# print("Average daily rets: " + str(fund_average_daily_rets))
	# print ("STD daily rets: " + str(fund_STD_daily_rets))
	# print ("Sharp Ratio: " + str(fund_sharp_ratio_daily))
	# print ("Cumm return: " + str(fund_cummulative_daily_rets))


if __name__ == '__main__':
	print 'Number of arguments:', len(sys.argv), 'arguments.'
	print 'Argument List:', str(sys.argv)
	marketSimulator(init_value=sys.argv[1], inFileName=sys.argv[2], outFileName=sys.argv[3])
	#print('##############################################################')
	#print('orders_short.csv')
	#marketSimulator(1000000, 'orders_short.csv', 'orders_short_out.csv')

	#print('##############################################################')
	#print('orders.csv')
	#marketSimulator(1000000, 'orders.csv', 'orders_out.csv')

	#print('##############################################################')
	#print('orders2.csv')
	#marketSimulator(1000000, 'orders2.csv', 'orders2_out.csv')

	#print('##############################################################')
	#print('Quizz 1: 5$ ')
	#marketSimulator(50000, '../Week6/sp500_5event.csv', '../Week6/sp500_5event_out.csv')
	# print('##############################################################')
	# print('Quizz 1: 10$ ')
	# marketSimulator(50000, '../Week6/sp500_10event.csv', '../Week6/sp500_10event_out.csv')
	# print('##############################################################')
	# print('Quizz 2: 8$ ')
	# marketSimulator(50000, '../Week6/sp500_8event.csv', '../Week6/sp500_8event_out.csv')

