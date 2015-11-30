import QSTK.qstkutil.qsdateutil as du
import QSTK.qstkutil.tsutil as tsu
import QSTK.qstkutil.DataAccess as da

import datetime as dt
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import math

def simulate(startdate, enddate, ls_symbols, ls_ratios):
    dt_timeofday = dt.timedelta(hours=16)
    ldt_timestamps = du.getNYSEdays(dt_start, dt_end, dt_timeofday)
    c_dataobj = da.DataAccess('Yahoo')
    ls_keys = ['close']
    ldf_data = c_dataobj.get_data(ldt_timestamps, ls_symbols, ls_keys)
    d_data = dict(zip(ls_keys, ldf_data))
    na_price = d_data['close'].values
    cumm_daily_rets = tsu.returnize1(na_price.copy())

    cumm_daily_price= cumm_daily_rets.copy()
    cumm_daily_price[0] =  cumm_daily_rets[0]*ls_ratios

    for idx  in xrange(1, len(cumm_daily_rets)):
        cumm_daily_price[idx] = cumm_daily_price[idx-1] * cumm_daily_rets[idx]

    portf_cumm_daily_values = np.sum(cumm_daily_price, axis = 1)
    portf_daily_rets = tsu.returnize0(portf_cumm_daily_values.copy())

    average_daily_rets = np.average(portf_daily_rets, axis = 0)
    STD_daily_rets = np.std(portf_daily_rets.copy(), axis = 0)
    sharp_ratio_daily = math.sqrt(252)* (average_daily_rets/STD_daily_rets)
    cummulative_daily_rets = (portf_cumm_daily_values[len(ldt_timestamps)-1] / portf_cumm_daily_values[0] )

    return  STD_daily_rets, average_daily_rets, sharp_ratio_daily, cummulative_daily_rets

def stepBack(stack, current_sum, idx) :
    if(idx > 0) :
        current_sum -= stack[idx]
        stack[idx] = 0
        idx -= 1
        stack[idx] += 1
        current_sum += 1
    return current_sum, idx

def generateRatios (ratioscount, capacity):
    idx = 0
    stack = [0]*ratioscount
    current_sum = 0


    while (stack[0] < capacity) :
        if(idx < ratioscount - 1 ) :
            if(stack[idx] <= capacity and current_sum < capacity) :
                idx += 1
                # current_sum, idx = stepForward(stack, current_sum, idx)
            else :
                current_sum, idx = stepBack(stack, current_sum, idx)
        else :
            if(current_sum < capacity) :
                stack[ratioscount - 1] = capacity - current_sum
                current_sum = capacity

            while (current_sum == capacity):
                yield stack
                current_sum, idx = stepBack(stack, current_sum, idx)
                if(idx == 0 and current_sum == capacity) :
                    break
    yield stack


# dt_start = dt.datetime(2011, 1, 1)
# dt_end = dt.datetime(2011, 12, 31)
# ls_symbols = ['AAPL', 'GLD', 'GOOG', 'XOM']
# ls_allocations = [0.4, 0.4, 0.0, 0.2]
# #vol, daily_ret, sharpe, cum_ret = \
# print(simulate(dt_start, dt_end, ls_symbols, ls_allocations))
#
# dt_start = dt.datetime(2010, 1, 1)
# dt_end = dt.datetime(2010, 12, 31)
# ls_symbols = ['AXP', 'HPQ', 'IBM', 'HNZ']
# ls_allocations = [0.0, 0.0, 0.0, 1.]
# #vol, daily_ret, sharpe, cum_ret = \
# print(simulate(dt_start, dt_end, ls_symbols, ls_allocations))



def computeBestAlloc(dt_start, dt_end, ls_symbols) :
    ls_ratio = [0.]*len(ls_symbols)
    best_vol = float("-Inf")
    best_ret = float("-Inf")
    best_sharpe = float("-Inf")
    best_cum_ret = float("-Inf")
    best_alloc = [float("-Inf")] * len(ls_symbols)
    for ratio in generateRatios(len(ls_symbols), 10) :
        for idx in xrange(0, len(ls_ratio)) :
            ls_ratio[idx] = ratio[idx]/10.
        vol, daily_ret, sharpe, cum_ret = simulate(dt_start, dt_end, ls_symbols, ls_ratio)

        if(sharpe > best_sharpe) :
            best_vol, best_ret, best_sharpe, best_cum_ret = vol, daily_ret, sharpe, cum_ret
            best_alloc = ls_ratio[:]

    return best_sharpe, best_alloc

print("Problem 1")
dt_start = dt.datetime(2010, 1, 1)
dt_end = dt.datetime(2010, 12, 31)
ls_symbols = ['AAPL', 'GOOG', 'IBM', 'MSFT']
best_sharpe, best_alloc = computeBestAlloc(dt_start, dt_end, ls_symbols)
print(best_sharpe)
#print(simulate(dt_start, dt_end, ls_symbols, best_alloc))

print("Problem 2")
dt_start = dt.datetime(2010, 1, 1)
dt_end = dt.datetime(2010, 12, 31)
ls_symbols = ['BRCM', 'TXN', 'IBM', 'HNZ']
best_sharpe, best_alloc = computeBestAlloc(dt_start, dt_end, ls_symbols)
print(best_alloc)
#print(simulate(dt_start, dt_end, ls_symbols, best_alloc))