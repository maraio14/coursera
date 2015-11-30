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



def boillingerValue(start_date, end_date, ls_symbols, ls_keys, days_period):
	#Finding the first day to extract data
	td = dt.timedelta(hours=16)
	td_day = dt.timedelta(days=1)

	start_date += td
	end_date += td
	ldt_timestamps_window = []
	ldt_timestamps_window.append(start_date)

	for idx in xrange(days_period - 1) :
		prev_date = ldt_timestamps_window[0] - td_day
	 	prev_date = du.getPrevNNYSEday(prev_date, td) #check valid day
	 	if prev_date is not None:
	 		ldt_timestamps_window.insert(0, prev_date)

	# td_days_period = dt.timedelta(days=days_period-1)
	# ldt_timestamps_window = du.getNYSEdays(start_date - td_days_period, start_date, td)

	ldt_timestamps_res = du.getNYSEdays(start_date, end_date, td)
	dt_results = [pd.DataFrame(np.zeros((len(ldt_timestamps_res), len(ls_symbols))), index=ldt_timestamps_res, columns=ls_symbols) for idx in ls_keys]

	ldt_timestamps_all = du.getNYSEdays(ldt_timestamps_window[0], end_date, td)

	dataobj = da.DataAccess('Yahoo')
	ldf_data = dataobj.get_data(ldt_timestamps_all, ls_symbols, ls_keys)



	d_data = dict(zip(ls_keys, ldf_data))
	for s_key in ls_keys:
		d_data[s_key] = d_data[s_key].fillna(method='ffill')
		d_data[s_key] = d_data[s_key].fillna(method='bfill')
		#d_data[s_key] = d_data[s_key].fillna(1.0)

	# #solution with rolling_mean and rolling_std
	# for key_idx in xrange(len(ls_keys)):
	# 	print((ldf_data[key_idx] - pd.rolling_mean(ldf_data[key_idx], window=days_period, min_periods=days_period) )/
	# 									pd.rolling_std(ldf_data[key_idx], window=days_period, min_periods=days_period))

	for timestamp in ldt_timestamps_all :
		if timestamp > start_date :
				ldt_timestamps_window.append(timestamp)
				if(len(ldt_timestamps_window) > days_period ):
					ldt_timestamps_window.pop(0)

		if timestamp >= start_date :
			assert(len(ldt_timestamps_window) == days_period)
			for key_idx in xrange(len(ls_keys)):
				for symbol in ls_symbols :
					vals = ldf_data[key_idx].ix[ldt_timestamps_window[:]][symbol].values
					mean = np.mean(vals, axis = 0)
					std = np.std(vals, ddof=1, dtype=np.float64) #ddof = 1 mean removing statistical noise for STD.
					#std = np.std(vals, dtype=np.float64)
					boillinger = (vals[-1] - mean) / std
					dt_results[key_idx].ix[ldt_timestamps_window[-1]][symbol] = boillinger

	return dict(zip(ls_keys, dt_results))


if __name__ == '__main__':
	print(boillingerValue(dt.datetime(2010, 12, 23), dt.datetime(2010, 12, 30), ['AAPL', 'GOOG', 'IBM', 'MSFT'], ['close'], 20))
	#print(boillingerValue(dt.datetime(2010, 05, 21), dt.datetime(2010, 05, 21), ['AAPL'], ['close'], 20))
	#print(boillingerValue(dt.datetime(2010, 06, 14), dt.datetime(2010, 06, 14), ['MSFT'], ['close'], 20))


