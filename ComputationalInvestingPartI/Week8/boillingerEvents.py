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

"""
Accepts a list of symbols along with start and end date
Returns the Event Matrix which is a pandas Datamatrix
Event matrix has the following structure :
    |IBM |GOOG|XOM |MSFT| GS | JP |
(d1)|nan |nan | 1  |nan |nan | 1  |
(d2)|nan | 1  |nan |nan |nan |nan |
(d3)| 1  |nan | 1  |nan | 1  |nan |
(d4)|nan |  1 |nan | 1  |nan |nan |
...................................
...................................
Also, d1 = start date
nan = no information about any event.
1 = status bit(positively confirms the event occurence)
"""


def findEvents(d_data):
	df_boillinger = d_data['close']
	ls_symbols = df_boillinger.columns
	df_events = copy.deepcopy(df_boillinger)
	df_events = df_events * np.NAN

	# Time stamps for the event range
	ldt_timestamps = df_boillinger.index

	for s_sym in ls_symbols:
		for i in range(1, len(ldt_timestamps) - 1):
			# Calculating the returns for this timestamp
			f_boillinger_spy = df_boillinger['SPY'].ix[ldt_timestamps[i]]
			f_boillinger_today = df_boillinger[s_sym].ix[ldt_timestamps[i]]
			f_boillinger_yest = df_boillinger[s_sym].ix[ldt_timestamps[i - 1]]

			# Event is found if the symbol is down more then 3% while the
			# market is up more then 2%
			# if f_symreturn_today <= -0.03 and f_marketreturn_today >= 0.02:
			if f_boillinger_yest >= -2. and f_boillinger_today < -2. and f_boillinger_spy >= 1.4:
				df_events[s_sym].ix[ldt_timestamps[i]] = 1

	return df_events


def boillingerValue(start_date, end_date, spProfile, ls_keys, days_period):
	# Finding the first day to extract data
	td = dt.timedelta(hours=16)
	td_day = dt.timedelta(days=1)

	start_date += td
	end_date += td
	ldt_timestamps_window = []
	ldt_timestamps_window.append(start_date)

	for idx in xrange(days_period - 1):
		prev_date = ldt_timestamps_window[0] - td_day
		prev_date = du.getPrevNNYSEday(prev_date, td)  #check valid day
		if prev_date is not None:
			ldt_timestamps_window.insert(0, prev_date)

	dataobj = da.DataAccess('Yahoo')
	ls_symbols = dataobj.get_symbols_from_list(spProfile)
	ls_symbols.append('SPY')

	ldt_timestamps_res = du.getNYSEdays(start_date, end_date, td)
	dt_results = [
		pd.DataFrame(np.zeros((len(ldt_timestamps_res), len(ls_symbols))), index=ldt_timestamps_res, columns=ls_symbols) for
		idx in ls_keys]

	ldt_timestamps_all = du.getNYSEdays(ldt_timestamps_window[0], end_date, td)

	ldf_data = dataobj.get_data(ldt_timestamps_all, ls_symbols, ls_keys)

	d_data = dict(zip(ls_keys, ldf_data))
	for s_key in ls_keys:
		d_data[s_key] = d_data[s_key].fillna(method='ffill')
		d_data[s_key] = d_data[s_key].fillna(method='bfill')
	#d_data[s_key] = d_data[s_key].fillna(1.0)

	# #solution with rolling_mean and rolling_std
	# for key_idx in xrange(len(ls_keys)):
	# 	print((ldf_data[key_idx] - pd.rolling_mean(ldf_data[key_idx], window=days_period, min_periods=days_period) )/
	#  									pd.rolling_std(ldf_data[key_idx], window=days_period, min_periods=days_period))

	for timestamp in ldt_timestamps_all:
		if timestamp > start_date:
			ldt_timestamps_window.append(timestamp)
			if (len(ldt_timestamps_window) > days_period ):
				ldt_timestamps_window.pop(0)

		if timestamp >= start_date:
			assert (len(ldt_timestamps_window) == days_period)
			for key_idx in xrange(len(ls_keys)):
				for symbol in ls_symbols:
					vals = ldf_data[key_idx].ix[ldt_timestamps_window[:]][symbol].values
					mean = np.mean(vals, axis=0)
					std = np.std(vals, ddof=1, dtype=np.float64)  #ddof = 1 mean removing statistical noise for STD.
					#std = np.std(vals, dtype=np.float64)
					boillinger = (vals[-1] - mean) / std
					dt_results[key_idx].ix[ldt_timestamps_window[-1]][symbol] = boillinger

	return dict(zip(ls_keys, dt_results))

def print_events(df_events, d_data):
	    ep.eventprofiler(df_events, d_data, i_lookback=20, i_lookforward=20,
                s_filename='events.pdf', b_market_neutral=True, b_errorbars=True,
                s_market_sym='SPY')

if __name__ == '__main__':
	d_data = boillingerValue(dt.datetime(2008, 1, 1), dt.datetime(2009, 12, 31), 'sp5002012', ['close'], 20)
	df_events = findEvents(d_data)
	print_events(df_events, d_data)

# print(boillingerValue(dt.datetime(2010, 05, 21), dt.datetime(2010, 05, 21), ['AAPL'], ['close'], 20))
#print(boillingerValue(dt.datetime(2010, 06, 14), dt.datetime(2010, 06, 14), ['MSFT'], ['close'], 20))


