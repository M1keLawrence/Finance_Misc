import math
import numpy as np
import functools
import itertools
from Arrow_Debreu_optionsMarket import Arrow_Debreu_options_model
from General_Functions import option_payoff_mat

# First, formating. 
# legacy='1.25' fixes np.float64(...) output
np.set_printoptions(precision=6,legacy='1.25')

# Question 1
put_data = [[1175,1200,1250,1350], [46.60,51.55,63.30,95.30] ]
call_data = [[1350,1375,1450,1550,1600], [99.55,84.90,47.25,15.80,7.90]]


market_states = [0,1187.5,1225,1300,1362.5,1412.5,1500,1575,0]
first_states = [800, 950 ,1100]
last_states = [1650,1700,1800]

combinations = list(itertools.product(first_states,last_states))

for combo in combinations:
    #print("\nNew Combo")
    # sort data into relvant put and call sections
    market_states[0] = combo[0]; market_states[-1] = combo[1]
    put_states = [S0 for S0 in market_states if S0 < max(put_data[0]) ]
    call_states = [S0 for S0 in market_states if S0 > min(call_data[0])]
    
    market_states_input = [put_states, call_states]

    
    AD_Model = Arrow_Debreu_options_model(put_data,call_data,market_states_input)

    #print(AD_Model.Get_Q())
    #AD_Model.check_Arbitrage()


# Question 2
Call_bid = [68,40.3,29.6,15,10,4,2.5,0.8,0.35]
Call_ask = [70,42.3,31.6,16.2,11,4.7,3.2,1.25,0.8]

Call_market_data = [[1175,1225,1250,1300,1325,1375,1400,1450,1475],
             [(Call_bid[i] + Call_ask[i])/2 for i in range(len(Call_ask))]]

Put_bid = [3.4,8.5,11.1,15.7,18,22.7,35.3,44.1]
Put_ask = [4.1,9.5,12.6,17.2,19.5,24.7,37.3,46.1]
Put_mid = [(Put_bid[i]+Put_ask[i])/2 for i in range(len(Put_ask))]

Put_strikes = [900,995,1025,1060,1075,1100,1150,1175]
Put_market_data = [Put_strikes,Put_mid]



call_model_data_mids = [(52.8+54.8)/2,(21.3+23.3)/2,(6.3+7.3)/2,(1.4+1.85)/2]
call_model_data = [[1200,1275,1350,1425], call_model_data_mids]


put_market_data_mids = [(1.2+1.65)/2,(5.3+6.3)/2,(14+15.5)/2,(53.9+55.9)/2]
put_model_data = [[800,950,1050,1200],put_market_data_mids]

market_states = [650,875,1000,1125,1237.5,1312.5,1387.5,1500]

put_states = [S0 for S0 in market_states if S0 < max(put_model_data[0]) ]
call_states = [S0 for S0 in market_states if S0 > min(call_model_data[0])]

market_states_input = [put_states, call_states]

AD_Model = Arrow_Debreu_options_model(put_model_data,call_model_data,market_states_input)

true_mids = Put_market_data[1] + Call_market_data[1]

Model_MAE = AD_Model.Model_MAE(Put_market_data[0],Call_market_data[0],true_mids)
# print(f"RMSE: {Model_MAE}")