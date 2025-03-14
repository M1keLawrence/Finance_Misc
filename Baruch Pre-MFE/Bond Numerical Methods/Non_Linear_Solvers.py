from GeneralFunctions import *
from Bond import Bond 
import functools
import math
import numpy as np

################################################################################
# Non-Linear solvers testing ###################################################
################################################################################
"""
f1 = lambda x: x
f1_prime = lambda x: 1

print("Bisection Method: ", NLin_Bisection(f1, -11, 10, math.pow(10,-6),math.pow(10,-9)))

f2 = lambda x: math.pow(x,4) - 5*x*x + 4 - 1/(1 + math.exp(x*x*x)) 
f2_prime = lambda x: 4*x*x*x - 10*x + 3*x*x*math.exp(x*x*x)*math.pow(1 + math.exp(x*x*x), -2)

x_old = -3
x_oldest = x_old - 0.01 

print("Newton's Method: ", NLin_Newtons_SingleVar(f2,f2_prime,x_old,math.pow(10,-6),math.pow(10,-9)))
print("Secant Method: ", NLin_Secant(f2,x_oldest, x_old,math.pow(10,-6),math.pow(10,-9)))
"""

################################################################################
# Computing a Bond's yield #####################################################
################################################################################

# Using functions 

def Bond_Price(y, t_cash_flow, v_cash_flow, use_case = 0):
    """
    Purpose: 
            Calculates price of bond. 
            Can use functools.partial to reduce to a function of single variable y for use in Non-Linear solvers
    Input: 
            y: yield of bond
            t_cash_flow: vector of cash flow dates
            v_cash_flow: vector of cash flows
            use_case: a signal for what use case is needed
                    - if 0: return normal price of bond
                    - else: return B - use_case; where use_case will be the market price of the bond for use in solving for the yield
    Output: 
            B: price of bond
    """
    B = 0
    for i in range(0,len(v_cash_flow)):
        B += v_cash_flow[i]*math.exp(-y*t_cash_flow[i])

    return B - use_case

def Duration(y, t_cash_flow, v_cash_flow): 
    """
    Purpose: 
            Calculates derivative of bond wrt the yield, y
            Can use functools.partial to reduce to a function of single variable y for use in Newton's Method. 
    Input: 
            y: yield of bond
            t_cash_flow: vector of cash flow dates
            v_cash_flow: vector of cash flows
    Output: 
            B: derivative of bond wrt the yield, y
    """
    dB_dy = 0
    for i in range(0,len(v_cash_flow)):
        dB_dy -= v_cash_flow[i]*t_cash_flow[i]*math.exp(-y*t_cash_flow[i])
    return dB_dy

def Convexity(y, t_cash_flow, v_cash_flow): 
    """
    Purpose: 
            Calculates derivative of bond wrt the yield, y
            Can use functools.partial to reduce to a function of single variable y for use in Newton's Method. 
    Input: 
            y: yield of bond
            t_cash_flow: vector of cash flow dates
            v_cash_flow: vector of cash flows
    Output: 
            B: derivative of bond wrt the yield, y
    """
    C = 0
    for i in range(0,len(v_cash_flow)):
        C = v_cash_flow[i]*t_cash_flow[i]*t_cash_flow[i]*math.exp(-y*t_cash_flow[i])
    return C

def compute_yield(B, x0, t_cash_flow, v_cash_flow, tol):
    """
    Inputs: 
            B: the price of the bond
            t_cash_flow: vector of cash flow dates
            v_cash_flow: vector of cash flows
            tol = tolerance for declaring convergence of Newtons Method
    Outputs: 
            x_new: the yield of the bond
    """
    B_y = functools.partial(Bond_Price, t_cash_flow = t_cash_flow, v_cash_flow = v_cash_flow, use_case = B)
    dB_dy_y = functools.partial(dB_dy, t_cash_flow = t_cash_flow, v_cash_flow = v_cash_flow)

    return NLin_Newtons_SingleVar(B_y, dB_dy_y,x0,tol,tol)

y = 0.09
coupon_timing = 3/12
coupon_rate = 0.08
expiration_date = 24/12
tol = math.pow(10,-6)
t_cash_flow = calc_cash_flow_times(coupon_timing, expiration_date)
v_cash_flow = calc_coupons(coupon_timing, coupon_rate,len(t_cash_flow))

x0 = 0.05
tol = math.pow(10,-6)
r_overnight = 0.05
r_half = 0.0506
r_one = 0.0444796
t_vec_1 = np.array([1.5,2,3])
func1 = lambda x: 2.5* math.exp(r_half*0.5) + 2.5*math.exp(r_one) +  2.5*math.exp((0.5*x/2 + 1.5*r_one/2)*1.5) + 2.5*math.exp((x/2 + r_one/2)*2) + 2.5*math.exp((1.5*x/2 + 0.5*r_one/2)*2.5) + 102.5*math.exp(x*3) - 102
func2 = lambda x: 2.5*0.5*math.exp(r_half*0.5) + 2.5*math.exp(r_one) +  2.5*1.5*math.exp((0.5*x/2 + 1.5*r_one/2)*1.5) + 2.5*2*math.exp((x/2 + r_one/2)*2) + 2.5*2.5*math.exp((1.5*x/2 + 0.5*r_one/2)*2.5) + 102.5*3*math.exp(x*3)
r_three = NLin_Newtons_SingleVar(func1,func2,x0,tol,tol)

func3 = lambda x: 3* math.exp(r_half*0.5) + 3*math.exp(r_one) +  3*math.exp((0.5*r_three/2 + 1.5*r_one/2)*1.5) + 3*math.exp((r_three/2 + r_one/2)*2) + 3*math.exp((1.5*r_three/2 + 0.5*r_one/2)*2.5) + 3*math.exp(r_three*3) + 3*math.exp((0.5*x/2 + 1.5*r_three/2)*3.5) + 3*math.exp((x/2 + r_three/2)*4) + 3*math.exp((1.5*x/2 + 0.5*r_three/2)*4.5) + 103*math.exp(x*5) - 104
func4 = lambda x: 3*0.5*math.exp(r_half*0.5) + 3*math.exp(r_one) +  3*1.5*math.exp((0.5*r_three/2 + 1.5*r_one/2)*1.5) + 3*2*math.exp((r_three/2 + r_one/2)*2) + 3*2.5*math.exp((1.5*r_three/2 + 0.5*r_one/2)*2.5) + 3*3*math.exp(r_three*3) + 3*3.5*math.exp((0.5*x/2 + 1.5*r_three/2)*3.5) + 3*4*math.exp((x/2 + r_three/2)*4) + 3*4.5*math.exp((1.5*x/2 + 0.5*r_three/2)*4.5) + 103*5*math.exp(x*5) - 104
r_five = NLin_Newtons_SingleVar(func3,func4,x0,tol,tol)
print(r_five)
#B = Bond_Price(y,t_cash_flow,v_cash_flow)
#D = Duration(y,t_cash_flow,v_cash_flow)
#C = Convexity(y,t_cash_flow,v_cash_flow)
#print(B,"\n", D,"\n",C)
#-182.91506013026083
#340.790246255799
#B_y = functools.partial(Bond_Price, t_cash_flow= t_cash_flow,v_cash_flow=v_cash_flow)
#y_jiggle = np.array([0.001,0.005,0.01,0.02,0.04])
#for dy in y_jiggle:
#    print(abs(B_y(y + dy) - 182.91506013026083*dy)/(B_y(y + dy)))
#print("convexity")
#for dy in y_jiggle:
#    print(abs(B_y(y + dy) - 182.91506013026083*dy + 0.5*340.790246255799*dy*dy)/B_y(y + dy))

#Computing a bond's yield using functions embedded into the class structure
'''
x0 = 0.1
B = 101
coupon_timing = 0.5
coupon_rate = 0.04
expiration_date = 3
tol = math.pow(10,-6)

B1 = Bond(expiration_date, B, coupon_timing, coupon_rate)
print(B1.t_cash_flows)
print(B1.v_cash_flows)
B1.calc_Yield(x0,tol)
print(B1.y)
print(B1.calc_Duration())
print(B1.calc_Convexity())
'''

#Computing the implied volatility of a call using functions
#I havn't the time to make another class, though the process will be smoother the second time around. 
'''

def call_Price(K,S,sig,q,r,T, market_price):
    d1 = (math.log(S/K) + (r-q+sig*sig/2.0)*T)/(sig*math.sqrt(T))
    d2 = d1 - sig*math.sqrt(T)
    return S*math.exp(-q*T)*Cum_Dist_StdNormal(d1) - K*math.exp(-r*T)*Cum_Dist_StdNormal(d2) - market_price

def put_Price(T,S,K,sig,q,r): 
    d1 = (math.log(S/K) + (r-q+sig*sig/2.0)*T)/(sig*math.sqrt(T))
    d2 = d1 - sig*math.sqrt(T)
    return K*math.exp(-r*T)*Cum_Dist_StdNormal(-d2) - S*math.exp(-q*T)*Cum_Dist_StdNormal(-d1)

def dC_dsig(K,S,sig,q,r,T, newtons_var = 0):
    d1 = (math.log(S/K) + (r-q+sig*sig/2)*T)/(sig*math.sqrt(T))
    return math.exp(-r*T)*Cum_Dist_StdNormal(d1) - newtons_var

def dC2_dsigdK(K,S,sig,q,r,T):
    d1 = (math.log(S/K) + (r-q+sig*sig/2)*T)/(sig*math.sqrt(T))
    return -math.exp(r*T*d1*d1/2)/(math.sqrt(2*math.pi*K*K*sig*sig*T))

def dP_dT(K,S,sig,q,r,T):
    d1 = (math.log(S/K) + (r-q+sig*sig/2.0)*T)/(sig*math.sqrt(T))
    d2 = d1 - sig*math.sqrt(T)
    return -S*sig*math.exp(-q*(T))*math.exp(-d1*d1/2)/(2*math.sqrt(2*math.pi*(T))) - q*S*math.exp(-q*T)*Cum_Dist_StdNormal(-d1) + r*K*math.exp(-r*T)*Cum_Dist_StdNormal(-d2) 


def option_Implied_vol(S,sig,q,r,T, x0, market_price, tol):
    """
    Inputs: 
            market_price: the price of the bond
            t_cash_flow: vector of cash flow dates
            v_cash_flow: vector of cash flows
            tol = tolerance for declaring convergence of Newtons Method
    Outputs: 
            x_new: the yield of the bond
    """
    f_sig = functools.partial(call_Price, S = S, sig=sig, q = q, r = r, T = T, market_price = market_price)
    df_dsig_sig = functools.partial(dC_dsig, S=S, sig=sig,q=q,r=r,T=T)

    return NLin_Newtons_SingleVar(f_sig, df_dsig_sig, x0, tol, tol)


#S = 45
#K = 45
#log_moneyness = S/K
#sig=0.25
#T=6/12
#r=0.02
#q=0.0
#x0 = 30
#step_vec = np.array([1/252,1/(2*252),1/(4*252),1/(8*252)])
#P_T = functools.partial(put_Price,S=S,K=K,sig=sig,r=r,q=q)
#Theta_P = np.empty_like(step_vec)

#for i in range(0,len(step_vec)):
#    Theta_P[i] = (P_T(T-step_vec[i]) - P_T(T))/(step_vec[i])

#print("Theta :", Theta_P)
'''

# Question 4 of HW5
#f_K = functools.partial(dC_dsig, S = S, sig=sig, q = q, r = r, T = T, newtons_var = delta_C) 
#df_dK = functools.partial(dC2_dsigdK,S=S,sig=sig,q=q,r=r,T=T) 
#K_imp = NLin_Newtons_SingleVar(f_K,df_dK,x0,tol,tol)
#print(K_imp)
'''
# Question 5 of HW5
def max_moneyness(x,r,q,sig,T):
    #print(x)
    d1 = (math.log(x) + (r-q+sig*sig/2.0)*T)/(sig*math.sqrt(T))
    d2 = d1 - sig*math.sqrt(T)
    return x - (2*r*math.sqrt(2*math.pi*T)/sig)*math.exp(d1*d1/2 - r*T)*Cum_Dist_StdNormal(-d2)

def dmax_moneyness_dSK(x,r,q,sig,T):
    d1 = (math.log(x) + (r-q+sig*sig/2.0)*T)/(sig*math.sqrt(T))
    d2 = d1 - sig*math.sqrt(T)
    return 1 - (2*r*math.sqrt(2*math.pi)*math.exp(d1*d1/2 - r*T)/(sig*sig*x))*(d1*Cum_Dist_StdNormal(-d2) - math.exp(-d2*d2/2)/math.sqrt(2*math.pi))

T = 6/12
sig = 0.3
r = 0.03
q = 0
x0 = 0.9
tol = math.pow(10,-6)
max_money = functools.partial(max_moneyness,r=r,q=q,sig=sig,T=T)
max_money_deriv = functools.partial(dmax_moneyness_dSK,r=r,q=q,sig=sig,T=T)
money_approx = NLin_Newtons_SingleVar(max_money,max_money_deriv,x0,tol,tol)
print(money_approx)


# Question 6 of HW5

def lin_interp(x1,x2,stop,start,t):
    return (t-start)*x1/(stop - start) + (start - t)*x2/(stop - start)

r_half = math.log(97.5/100)*-2
r_one = -math.log((100 - 2.5*math.exp(-r_half/2))/102.5)

t_vec = [i/2 + 1 for i in range(0,3)] 
start = 1
stop = 3
r_start = r_one
r_stop = lambda x: x
interp = functools.partial(lin_interp,x2 = r_start,start=start,stop = stop,t )d
'''