__author__ = 'justme'

import pandas as pd
import numpy as np
import math
import copy
import QSTK.qstkutil.qsdateutil as du
import datetime as dt
import QSTK.qstkutil.DataAccess as da
import QSTK.qstkutil.tsutil as tsu
import QSTK.qstkstudy.EventProfiler as ep
import csv


def find_events(ls_symbols, d_data, column_event, outFileName,event_value):
	''' Finding the event dataframe '''
	df_close = d_data[column_event]
	# ts_market = df_close['SPY']

	print('Finding Events')

	# Time stamps for the event range
	ldt_timestamps = df_close.index

	writer = csv.writer(open(outFileName, 'w'), delimiter=',')


	for s_sym in ls_symbols:
		for i in range(1, len(ldt_timestamps)):
			# Calculating the returns for this timestamp
			f_symprice_today = df_close[s_sym].ix[ldt_timestamps[i]]
			f_symprice_yest = df_close[s_sym].ix[ldt_timestamps[i - 1]]

			if ( (f_symprice_yest >= event_value and f_symprice_today < event_value)) :
				ldt_day = ldt_timestamps[i]
				writer.writerow((ldt_day.year, ldt_day.month, ldt_day.day, s_sym, 'Buy', 100))

				if i < len(ldt_timestamps) - 5:
					ldt_day = ldt_timestamps[i + 5]
					writer.writerow((ldt_day.year, ldt_day.month, ldt_day.day, s_sym, 'Sell', 100))
				else :
					ldt_day = ldt_timestamps[len(ldt_timestamps)-1]
					writer.writerow((ldt_day.year, ldt_day.month, ldt_day.day, s_sym, 'Sell', 100))
	return

def EventProfile(dt_start, dt_end, spProfile, outFileName, event_value):
	ldt_timestamps = du.getNYSEdays(dt_start, dt_end, dt.timedelta(hours=16))

	dataobj = da.DataAccess('Yahoo')
	ls_symbols = dataobj.get_symbols_from_list(spProfile)
	#ls_symbols.append('SPY')

	ls_keys = ['open', 'high', 'low', 'close', 'volume', 'actual_close']
	# ls_keys = ['actual_close']
	ldf_data = dataobj.get_data(ldt_timestamps, ls_symbols, ls_keys)
	d_data = dict(zip(ls_keys, ldf_data))

	for s_key in ls_keys:
		d_data[s_key] = d_data[s_key].fillna(method='ffill')
		d_data[s_key] = d_data[s_key].fillna(method='bfill')
		d_data[s_key] = d_data[s_key].fillna(1.0)

	df_events = find_events(ls_symbols, d_data, 'actual_close', outFileName, event_value)
	print "Creating Study"



if __name__ == '__main__':
	dt_start = dt.datetime(2008, 1, 1)
	dt_end = dt.datetime(2009, 12, 31)
	# EventProfile(dt_start, dt_end, 'sp5002008')
	# EventProfile(dt_start, dt_end, 'sp5002012', 'sp500_5event.csv', 5.)
	# EventProfile(dt_start, dt_end, 'sp5002012', 'sp500_7event.csv', 7.)
	# EventProfile(dt_start, dt_end, 'sp5002012', 'sp500_9event.csv', 9.)
	EventProfile(dt_start, dt_end, 'sp5002012', 'sp500_10event.csv', 10.)
	EventProfile(dt_start, dt_end, 'sp5002012', 'sp500_8event.csv', 8.)